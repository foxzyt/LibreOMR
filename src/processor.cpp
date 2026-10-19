/*
 * LibreOMR - Robust Optical Mark Recognition Library
 * Copyright (C) 2026 Bernardo Alvim
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "libreomr/processor.hpp"
#include "libreomr/binarization.hpp"
#include <algorithm>
#include <cmath>

namespace libreomr {

Processor::Processor() = default;

Processor::Processor(ProcessorConfig config) : config_(config) {}

BubbleMetric Processor::analyzeBubble(const Image& gray_image, const Image& binary_image,
                                      const BubbleDefinition& bubble) const {
    BubbleMetric m;
    m.label = bubble.label;
    m.center = bubble.center;
    m.bounds = bubble.boundingBox();

    int cx = static_cast<int>(std::round(bubble.center.x));
    int cy = static_cast<int>(std::round(bubble.center.y));
    float r = bubble.radius;

    float inner_r = r * config_.inner_sample_ratio;
    float inner_r2 = inner_r * inner_r;

    float bg_in = r * config_.background_inner_ratio;
    float bg_out = r * config_.background_outer_ratio;
    float bg_in2 = bg_in * bg_in;
    float bg_out2 = bg_out * bg_out;

    uint32_t inner_pixels = 0;
    uint32_t dark_pixels = 0;
    double inner_intensity_sum = 0.0;

    uint32_t bg_pixels = 0;
    double bg_intensity_sum = 0.0;

    int max_radius = static_cast<int>(std::ceil(bg_out));
    int min_y = std::max(0, cy - max_radius);
    int max_y = std::min(static_cast<int>(gray_image.height()) - 1, cy + max_radius);
    int min_x = std::max(0, cx - max_radius);
    int max_x = std::min(static_cast<int>(gray_image.width()) - 1, cx + max_radius);

    for (int y = min_y; y <= max_y; ++y) {
        int dy = y - cy;
        int dy2 = dy * dy;

        for (int x = min_x; x <= max_x; ++x) {
            int dx = x - cx;
            float d2 = static_cast<float>(dx * dx + dy2);

            uint8_t gray_val = gray_image.at(static_cast<uint32_t>(x), static_cast<uint32_t>(y), 0);

            // Sample inner bubble
            if (d2 <= inner_r2) {
                inner_pixels++;
                inner_intensity_sum += gray_val;
                if (!binary_image.empty()) {
                    uint8_t bin_val = binary_image.at(static_cast<uint32_t>(x), static_cast<uint32_t>(y), 0);
                    if (bin_val == 0) { // Black/dark mark
                        dark_pixels++;
                    }
                }
            }
            // Sample outer background ring
            else if (d2 >= bg_in2 && d2 <= bg_out2) {
                bg_pixels++;
                bg_intensity_sum += gray_val;
            }
        }
    }

    if (inner_pixels > 0) {
        m.fill_ratio = static_cast<float>(dark_pixels) / static_cast<float>(inner_pixels);
        m.mean_intensity = static_cast<float>(inner_intensity_sum / inner_pixels);
    } else {
        m.fill_ratio = 0.0f;
        m.mean_intensity = 255.0f;
    }

    if (bg_pixels > 0) {
        m.local_background = static_cast<float>(bg_intensity_sum / bg_pixels);
    } else {
        m.local_background = 255.0f;
    }

    // Relative contrast against paper
    if (m.local_background > 10.0f) {
        m.relative_fill = std::clamp((m.local_background - m.mean_intensity) / m.local_background, 0.0f, 1.0f);
    } else {
        m.relative_fill = m.fill_ratio;
    }

    // Determine marked state
    if (config_.adaptive_thresholding) {
        m.is_marked = (m.fill_ratio >= config_.fill_threshold) || (m.relative_fill >= config_.fill_threshold);
    } else {
        m.is_marked = (m.fill_ratio >= config_.fill_threshold);
    }

    return m;
}

PageResult Processor::process(const Image& image, const Template& tmpl) const {
    PageResult result;
    result.alignment_successful = false;
    result.alignment_skew_degrees = 0.0f;
    result.detected_fiducials = tmpl.fiducials();
    result.template_fiducials = tmpl.fiducials();

    if (image.empty() || tmpl.width() == 0 || tmpl.height() == 0) {
        float total_possible = 0.0f;
        for (const auto& q_def : tmpl.questions()) {
            QuestionResult q_res;
            q_res.question_id = q_def.id;
            q_res.question_name = q_def.name;
            q_res.type = q_def.type;
            q_res.max_score = q_def.weight;
            q_res.is_answered = false;
            q_res.is_correct = false;
            q_res.is_ambiguous = false;
            q_res.confidence = 0.0f;
            total_possible += q_def.weight;
            result.questions.push_back(q_res);
        }
        result.total_score = 0.0f;
        result.max_score = total_possible;
        result.score_percentage = 0.0f;
        return result;
    }

    Image gray = (image.channels() > 1) ? image.toGrayscale() : image.clone();
    Image aligned = gray;

    if (config_.auto_align) {
        auto detected_quad = FiducialDetector::detectCorners(gray, config_.fiducial_config);
        if (detected_quad.has_value()) {
            result.detected_fiducials = *detected_quad;
            result.alignment_successful = true;

            // Compute skew angle from top markers
            double dx = detected_quad->top_right.x - detected_quad->top_left.x;
            double dy = detected_quad->top_right.y - detected_quad->top_left.y;
            double angle_rad = std::atan2(dy, dx);
            result.alignment_skew_degrees = static_cast<float>(angle_rad * 180.0 / 3.14159265358979323846);

            // Compute homography from detected image markers to template canonical positions
            bool h_ok = false;
            Matrix3x3 homography = Matrix3x3::computeHomography(*detected_quad, tmpl.fiducials(), &h_ok);
            if (h_ok) {
                // Warp detected page into canonical template coordinates
                aligned = gray.warpPerspective(homography, tmpl.width(), tmpl.height());
            }
        }
    }

    if (!result.alignment_successful) {
        // Fallback: If sizes differ, resize or use template fiducials as placeholder
        result.detected_fiducials = tmpl.fiducials();
        if (aligned.width() != tmpl.width() || aligned.height() != tmpl.height()) {
            Quad2D src_quad(Point2Df(0, 0), Point2Df(static_cast<float>(aligned.width()), 0),
                            Point2Df(static_cast<float>(aligned.width()), static_cast<float>(aligned.height())),
                            Point2Df(0, static_cast<float>(aligned.height())));
            Quad2D dst_quad(Point2Df(0, 0), Point2Df(static_cast<float>(tmpl.width()), 0),
                            Point2Df(static_cast<float>(tmpl.width()), static_cast<float>(tmpl.height())),
                            Point2Df(0, static_cast<float>(tmpl.height())));
            Matrix3x3 scale_h = Matrix3x3::computeHomography(src_quad, dst_quad);
            aligned = aligned.warpPerspective(scale_h, tmpl.width(), tmpl.height());
        }
    }

    // Binarize aligned image for bubble mark detection
    Image binary = Binarizer::otsu(aligned);

    float total_earned = 0.0f;
    float total_possible = 0.0f;

    for (const auto& q_def : tmpl.questions()) {
        QuestionResult q_res;
        q_res.question_id = q_def.id;
        q_res.question_name = q_def.name;
        q_res.type = q_def.type;
        q_res.max_score = q_def.weight;
        total_possible += q_def.weight;

        // Analyze all bubbles
        for (const auto& b_def : q_def.bubbles) {
            BubbleMetric bm = analyzeBubble(aligned, binary, b_def);
            q_res.bubbles.push_back(bm);
        }

        if (q_def.type == LIBREOMR_QUESTION_SINGLE_CHOICE) {
            // Find top and second top by fill_ratio
            int best_idx = -1;
            int second_idx = -1;
            float max_fill = -1.0f;
            float second_fill = -1.0f;

            for (size_t i = 0; i < q_res.bubbles.size(); ++i) {
                float fill = std::max(q_res.bubbles[i].fill_ratio, q_res.bubbles[i].relative_fill);
                if (fill > max_fill) {
                    second_fill = max_fill;
                    second_idx = best_idx;
                    max_fill = fill;
                    best_idx = static_cast<int>(i);
                } else if (fill > second_fill) {
                    second_fill = fill;
                    second_idx = static_cast<int>(i);
                }
            }

            if (best_idx >= 0 && max_fill >= config_.fill_threshold) {
                q_res.is_answered = true;
                q_res.selected_answer = q_res.bubbles[best_idx].label;

                if (second_idx >= 0 && second_fill >= config_.fill_threshold) {
                    // Multiple filled bubbles on single-choice question
                    q_res.is_ambiguous = true;
                    q_res.confidence = 0.0f;
                } else if (second_idx >= 0 && (max_fill - second_fill) < config_.ambiguity_margin) {
                    // Gap between top answer and runner-up is too narrow (e.g. erasure mark)
                    q_res.is_ambiguous = true;
                    q_res.confidence = std::clamp((max_fill - second_fill) / config_.ambiguity_margin, 0.0f, 1.0f);
                } else {
                    q_res.is_ambiguous = false;
                    float separation = (second_idx >= 0) ? (max_fill - second_fill) : max_fill;
                    q_res.confidence = std::clamp(separation * 1.5f, 0.0f, 1.0f);
                }
            } else {
                // Blank question
                q_res.is_answered = false;
                q_res.selected_answer = "";
                q_res.confidence = (best_idx >= 0) ? (1.0f - max_fill) : 1.0f;
                q_res.is_ambiguous = false;
            }
        } else {
            // Multi-choice question
            std::string combined;
            int marked_count = 0;
            float min_marked_fill = 1.0f;

            for (const auto& b : q_res.bubbles) {
                if (b.is_marked) {
                    if (!combined.empty()) combined += ",";
                    combined += b.label;
                    marked_count++;
                    min_marked_fill = std::min(min_marked_fill, b.fill_ratio);
                }
            }

            q_res.selected_answer = combined;
            q_res.is_answered = (marked_count > 0);
            q_res.is_ambiguous = false;
            q_res.confidence = (marked_count > 0) ? min_marked_fill : 1.0f;
        }

        // Grading against answer key
        if (!q_def.correct_answer.empty()) {
            if (q_res.is_answered && !q_res.is_ambiguous && q_res.selected_answer == q_def.correct_answer) {
                q_res.is_correct = true;
                q_res.earned_score = q_def.weight;
                total_earned += q_def.weight;
            } else {
                q_res.is_correct = false;
                q_res.earned_score = 0.0f;
            }
        }

        result.questions.push_back(q_res);
    }

    result.total_score = total_earned;
    result.max_score = total_possible;
    result.score_percentage = (total_possible > 0.0f) ? (total_earned / total_possible * 100.0f) : 0.0f;

    return result;
}

PageResult Processor::process(const std::string& image_path, const Template& tmpl) const {
    Image img = Image::loadFromFile(image_path);
    return process(img, tmpl);
}

} // namespace libreomr
