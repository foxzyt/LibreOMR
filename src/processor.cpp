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

    float r = bubble.radius;
    float cx_f = bubble.center.x;
    float cy_f = bubble.center.y;

    // Feature 3: Paper Curvature / Wrinkle Compensation (Local Center Refinement)
    if (config_.refine_bubble_centers && !binary_image.empty()) {
        int base_x = static_cast<int>(std::round(cx_f));
        int base_y = static_cast<int>(std::round(cy_f));
        int search_r = std::max(2, static_cast<int>(std::round(r * 0.35f)));

        double dark_sum = 0.0;
        double sum_x = 0.0;
        double sum_y = 0.0;

        for (int dy = -search_r; dy <= search_r; ++dy) {
            for (int dx = -search_r; dx <= search_r; ++dx) {
                int px = base_x + dx;
                int py = base_y + dy;
                if (px >= 0 && px < static_cast<int>(binary_image.width()) &&
                    py >= 0 && py < static_cast<int>(binary_image.height())) {
                    if (binary_image.at(px, py, 0) == 0) {
                        dark_sum += 1.0;
                        sum_x += px;
                        sum_y += py;
                    }
                }
            }
        }

        // Only refine center if there is a substantial ink mark (at least 15% of bubble area)
        // to avoid drifting onto printed letter glyphs (e.g. 'A', 'B', 'C') in empty bubbles.
        double min_ink_mass = std::max(16.0, 0.15 * 3.14159265358979323846 * r * r);
        if (dark_sum >= min_ink_mass) {
            float cand_x = static_cast<float>(sum_x / dark_sum);
            float cand_y = static_cast<float>(sum_y / dark_sum);
            float shift = std::hypot(cand_x - cx_f, cand_y - cy_f);
            if (shift <= static_cast<float>(search_r)) {
                cx_f = cand_x;
                cy_f = cand_y;
                m.center = Point2Df(cx_f, cy_f);
                m.bounds = Rectf(cx_f - r, cy_f - r, r * 2.0f, r * 2.0f);
            }
        }
    }

    int cx = static_cast<int>(std::round(cx_f));
    int cy = static_cast<int>(std::round(cy_f));

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
            Quad2D raw_quad = *detected_quad;
            Quad2D best_quad = raw_quad;

            // Feature 4: Automatic Orientation & EXIF Rotation (0, 90, 180, 270 deg)
            if (config_.auto_orient) {
                std::array<Quad2D, 4> candidate_quads = {
                    Quad2D(raw_quad.top_left, raw_quad.top_right, raw_quad.bottom_right, raw_quad.bottom_left),
                    Quad2D(raw_quad.bottom_left, raw_quad.top_left, raw_quad.top_right, raw_quad.bottom_right),
                    Quad2D(raw_quad.bottom_right, raw_quad.bottom_left, raw_quad.top_left, raw_quad.top_right),
                    Quad2D(raw_quad.top_right, raw_quad.bottom_right, raw_quad.bottom_left, raw_quad.top_left)
                };

                float tmpl_ar = static_cast<float>(tmpl.width()) / std::max(1.0f, static_cast<float>(tmpl.height()));
                bool tmpl_is_portrait = (tmpl_ar < 1.0f);

                int best_k = 0;
                double best_score = -1e9;

                for (int k = 0; k < 4; ++k) {
                    const auto& q = candidate_quads[k];
                    float q_w = q.top_left.distanceTo(q.top_right);
                    float q_h = q.top_left.distanceTo(q.bottom_left);
                    if (q_w < 10.0f || q_h < 10.0f) continue;
                    float q_ar = q_w / q_h;
                    bool q_is_portrait = (q_ar < 1.0f);

                    double score = 0.0;
                    if (q_is_portrait == tmpl_is_portrait) {
                        score += 500.0;
                    } else {
                        score -= 500.0;
                    }

                    bool h_inv_ok = false;
                    Matrix3x3 h_inv = Matrix3x3::computeHomography(tmpl.fiducials(), q, &h_inv_ok);
                    if (!h_inv_ok) continue;

                    // Sample header (top 12%) vs footer (bottom 10%)
                    double header_darkness = 0.0;
                    int header_samples = 0;
                    for (float px = 0.25f; px <= 0.75f; px += 0.15f) {
                        for (float py = 0.03f; py <= 0.12f; py += 0.03f) {
                            Point2Df probe_tmpl(px * tmpl.width(), py * tmpl.height());
                            Point2Df probe_img = h_inv.transform(probe_tmpl);
                            int ix = static_cast<int>(std::round(probe_img.x));
                            int iy = static_cast<int>(std::round(probe_img.y));
                            if (ix >= 0 && ix < static_cast<int>(gray.width()) &&
                                iy >= 0 && iy < static_cast<int>(gray.height())) {
                                header_darkness += (255.0 - gray.at(ix, iy, 0));
                                header_samples++;
                            }
                        }
                    }

                    double footer_darkness = 0.0;
                    int footer_samples = 0;
                    for (float px = 0.25f; px <= 0.75f; px += 0.15f) {
                        for (float py = 0.90f; py <= 0.97f; py += 0.03f) {
                            Point2Df probe_tmpl(px * tmpl.width(), py * tmpl.height());
                            Point2Df probe_img = h_inv.transform(probe_tmpl);
                            int ix = static_cast<int>(std::round(probe_img.x));
                            int iy = static_cast<int>(std::round(probe_img.y));
                            if (ix >= 0 && ix < static_cast<int>(gray.width()) &&
                                iy >= 0 && iy < static_cast<int>(gray.height())) {
                                footer_darkness += (255.0 - gray.at(ix, iy, 0));
                                footer_samples++;
                            }
                        }
                    }

                    double avg_header = header_samples > 0 ? (header_darkness / header_samples) : 0.0;
                    double avg_footer = footer_samples > 0 ? (footer_darkness / footer_samples) : 0.0;
                    score += (avg_header - avg_footer) * 5.0;

                    // Sample bubble locations for circular marks
                    int bubble_probes = 0;
                    double bubble_score = 0.0;
                    for (const auto& q_def : tmpl.questions()) {
                        for (const auto& b : q_def.bubbles) {
                            Point2Df center_img = h_inv.transform(b.center);
                            Point2Df edge_img = h_inv.transform(Point2Df(b.center.x + b.radius, b.center.y));
                            float r_img = center_img.distanceTo(edge_img);

                            int ix = static_cast<int>(std::round(center_img.x));
                            int iy = static_cast<int>(std::round(center_img.y));
                            int r_i = std::max(2, static_cast<int>(std::round(r_img)));
                            int bg_r = std::max(r_i + 3, static_cast<int>(std::round(r_img * 1.5f)));

                            if (ix - bg_r >= 0 && ix + bg_r < static_cast<int>(gray.width()) &&
                                iy - bg_r >= 0 && iy + bg_r < static_cast<int>(gray.height())) {
                                uint8_t center_val = gray.at(ix, iy, 0);

                                // Sample 4 points on circle perimeter
                                uint8_t p1 = gray.at(ix + r_i, iy, 0);
                                uint8_t p2 = gray.at(ix - r_i, iy, 0);
                                uint8_t p3 = gray.at(ix, iy + r_i, 0);
                                uint8_t p4 = gray.at(ix, iy - r_i, 0);
                                float perim_avg = (p1 + p2 + p3 + p4) / 4.0f;

                                // Sample 4 points on outer background
                                uint8_t b1 = gray.at(ix + bg_r, iy, 0);
                                uint8_t b2 = gray.at(ix - bg_r, iy, 0);
                                uint8_t b3 = gray.at(ix, iy + bg_r, 0);
                                uint8_t b4 = gray.at(ix, iy - bg_r, 0);
                                float bg_avg = (b1 + b2 + b3 + b4) / 4.0f;

                                // If filled: center is dark relative to outer background
                                float filled_contrast = bg_avg - center_val;
                                // If empty circle: perimeter is dark relative to both center and outer background
                                float circle_contrast = (bg_avg - perim_avg) + (center_val - perim_avg);

                                float feat = std::max(filled_contrast, circle_contrast);
                                if (feat > 0.0f) {
                                    bubble_score += feat;
                                }
                                bubble_probes++;
                            }
                            if (bubble_probes >= 15) break;
                        }
                        if (bubble_probes >= 15) break;
                    }
                    if (bubble_probes > 0) {
                        score += (bubble_score / bubble_probes) * 2.0;
                    }

                    if (score > best_score) {
                        best_score = score;
                        best_k = k;
                    }
                }

                best_quad = candidate_quads[best_k];
            }

            result.detected_fiducials = best_quad;
            result.alignment_successful = true;

            // Compute skew angle from top markers
            double dx = best_quad.top_right.x - best_quad.top_left.x;
            double dy = best_quad.top_right.y - best_quad.top_left.y;
            double angle_rad = std::atan2(dy, dx);
            result.alignment_skew_degrees = static_cast<float>(angle_rad * 180.0 / 3.14159265358979323846);

            // Compute homography from detected image markers to template canonical positions
            bool h_ok = false;
            Matrix3x3 homography = Matrix3x3::computeHomography(best_quad, tmpl.fiducials(), &h_ok);
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

    // Feature 2: Shadow Invariance via Adaptive Thresholding
    Image binary = config_.adaptive_thresholding
        ? Binarizer::adaptiveThreshold(aligned, 65, 12)
        : Binarizer::otsu(aligned);

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
