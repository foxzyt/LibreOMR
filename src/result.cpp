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

#include "libreomr/result.hpp"
#include "libreomr/json.hpp"
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>

namespace libreomr {

std::string PageResult::toJson() const {
    JsonValue root(JsonType::Object);
    root.set("alignment_successful", JsonValue(alignment_successful));
    root.set("alignment_skew_degrees", JsonValue(alignment_skew_degrees));

    JsonValue fids(JsonType::Object);
    auto makePt = [](const Point2Df& pt) {
        JsonValue p(JsonType::Object);
        p.set("x", JsonValue(pt.x));
        p.set("y", JsonValue(pt.y));
        return p;
    };
    fids.set("top_left", makePt(detected_fiducials.top_left));
    fids.set("top_right", makePt(detected_fiducials.top_right));
    fids.set("bottom_right", makePt(detected_fiducials.bottom_right));
    fids.set("bottom_left", makePt(detected_fiducials.bottom_left));
    root.set("detected_fiducials", fids);

    root.set("total_score", JsonValue(total_score));
    root.set("max_score", JsonValue(max_score));
    root.set("score_percentage", JsonValue(score_percentage));

    JsonValue q_arr(JsonType::Array);
    for (const auto& q : questions) {
        JsonValue q_obj(JsonType::Object);
        q_obj.set("question_id", JsonValue(q.question_id));
        q_obj.set("question_name", JsonValue(q.question_name));
        q_obj.set("type", JsonValue(static_cast<int>(q.type)));
        q_obj.set("selected_answer", JsonValue(q.selected_answer));
        q_obj.set("confidence", JsonValue(q.confidence));
        q_obj.set("is_answered", JsonValue(q.is_answered));
        q_obj.set("is_ambiguous", JsonValue(q.is_ambiguous));
        q_obj.set("is_correct", JsonValue(q.is_correct));
        q_obj.set("earned_score", JsonValue(q.earned_score));
        q_obj.set("max_score", JsonValue(q.max_score));

        JsonValue b_arr(JsonType::Array);
        for (const auto& b : q.bubbles) {
            JsonValue b_obj(JsonType::Object);
            b_obj.set("label", JsonValue(b.label));
            b_obj.set("fill_ratio", JsonValue(b.fill_ratio));
            b_obj.set("mean_intensity", JsonValue(b.mean_intensity));
            b_obj.set("local_background", JsonValue(b.local_background));
            b_obj.set("relative_fill", JsonValue(b.relative_fill));
            b_obj.set("is_marked", JsonValue(b.is_marked));
            b_arr.pushBack(b_obj);
        }
        q_obj.set("bubbles", b_arr);
        q_arr.pushBack(q_obj);
    }
    root.set("questions", q_arr);

    return root.serialize(2);
}

std::string PageResult::toCsv() const {
    std::ostringstream ss;
    ss << "question_id,question_name,selected_answer,is_answered,is_ambiguous,is_correct,earned_score,max_score,confidence\n";
    for (const auto& q : questions) {
        ss << "\"" << q.question_id << "\","
           << "\"" << q.question_name << "\","
           << "\"" << q.selected_answer << "\","
           << (q.is_answered ? "true" : "false") << ","
           << (q.is_ambiguous ? "true" : "false") << ","
           << (q.is_correct ? "true" : "false") << ","
           << q.earned_score << ","
           << q.max_score << ","
           << q.confidence << "\n";
    }
    return ss.str();
}

bool PageResult::saveToJsonFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) return false;
    file << toJson();
    return file.good();
}

bool PageResult::saveToCsvFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) return false;
    file << toCsv();
    return file.good();
}

Image PageResult::renderDebugImage(const Image& base_image) const {
    if (base_image.empty()) return Image();

    Image overlay = (base_image.channels() == 1) ? base_image.toRGB() : base_image.clone();

    // Blue for fiducial quad
    const std::vector<uint8_t> blue = {30, 120, 240};
    overlay.drawQuad(detected_fiducials, blue, 3);

    const std::vector<uint8_t> green = {0, 210, 50};
    const std::vector<uint8_t> orange = {255, 150, 0};
    const std::vector<uint8_t> light_red = {230, 70, 70};

    // Determine coordinate mapping from template canonical space to base_image space
    bool has_transform = false;
    Matrix3x3 tmpl_to_base;
    if (alignment_successful && template_fiducials.area() > 1.0 && detected_fiducials.area() > 1.0) {
        bool h_ok = false;
        tmpl_to_base = Matrix3x3::computeHomography(template_fiducials, detected_fiducials, &h_ok);
        has_transform = h_ok;
    }

    for (const auto& q : questions) {
        for (const auto& b : q.bubbles) {
            Point2Df center_pt = b.center;
            float r = b.bounds.width / 2.0f;

            if (has_transform) {
                Point2Df mapped_center = tmpl_to_base.transform(center_pt);
                Point2Df mapped_edge = tmpl_to_base.transform({center_pt.x + r, center_pt.y});
                r = mapped_center.distanceTo(mapped_edge);
                center_pt = mapped_center;
            }

            Point2Di center(static_cast<int>(std::round(center_pt.x)),
                            static_cast<int>(std::round(center_pt.y)));
            int draw_r = std::max(1, static_cast<int>(std::round(r)));

            if (b.is_marked) {
                if (q.is_ambiguous) {
                    overlay.drawCircle(center, draw_r + 2, orange, 3);
                } else {
                    overlay.drawCircle(center, draw_r + 2, green, 3);
                }
            } else {
                overlay.drawCircle(center, draw_r, light_red, 1);
            }
        }
    }

    return overlay;
}

libreomr_page_result_t* PageResult::toC() const {
    auto* c_res = static_cast<libreomr_page_result_t*>(std::calloc(1, sizeof(libreomr_page_result_t)));
    if (!c_res) return nullptr;

    c_res->alignment_successful = alignment_successful;
    c_res->alignment_skew_degrees = alignment_skew_degrees;
    c_res->detected_markers = detected_fiducials.toC();
    c_res->template_markers = template_fiducials.toC();
    c_res->total_score = total_score;
    c_res->max_score = max_score;
    c_res->score_percentage = score_percentage;
    c_res->num_questions = static_cast<uint32_t>(questions.size());

    if (c_res->num_questions > 0) {
        c_res->questions = static_cast<libreomr_question_result_t*>(
            std::calloc(c_res->num_questions, sizeof(libreomr_question_result_t)));
        if (!c_res->questions) {
            std::free(c_res);
            return nullptr;
        }

        for (size_t i = 0; i < questions.size(); ++i) {
            const auto& q = questions[i];
            auto& cq = c_res->questions[i];

            std::strncpy(cq.question_id, q.question_id.c_str(), sizeof(cq.question_id) - 1);
            std::strncpy(cq.question_name, q.question_name.c_str(), sizeof(cq.question_name) - 1);
            std::strncpy(cq.selected_answer, q.selected_answer.c_str(), sizeof(cq.selected_answer) - 1);

            cq.confidence = q.confidence;
            cq.is_answered = q.is_answered;
            cq.is_ambiguous = q.is_ambiguous;
            cq.is_correct = q.is_correct;
            cq.earned_score = q.earned_score;
            cq.max_score = q.max_score;
            cq.num_bubbles = static_cast<uint32_t>(q.bubbles.size());

            if (cq.num_bubbles > 0) {
                cq.bubbles = static_cast<libreomr_bubble_metric_t*>(
                    std::calloc(cq.num_bubbles, sizeof(libreomr_bubble_metric_t)));
                if (!cq.bubbles) {
                    freeC(c_res);
                    return nullptr;
                }

                for (size_t j = 0; j < q.bubbles.size(); ++j) {
                    const auto& b = q.bubbles[j];
                    auto& cb = cq.bubbles[j];

                    std::strncpy(cb.label, b.label.c_str(), sizeof(cb.label) - 1);
                    cb.fill_ratio = b.fill_ratio;
                    cb.mean_intensity = b.mean_intensity;
                    cb.relative_fill = b.relative_fill;
                    cb.is_marked = b.is_marked;
                    cb.bounds = b.bounds.toC();
                }
            }
        }
    }

    return c_res;
}

void PageResult::freeC(libreomr_page_result_t* c_res) {
    if (!c_res) return;

    if (c_res->questions) {
        for (uint32_t i = 0; i < c_res->num_questions; ++i) {
            if (c_res->questions[i].bubbles) {
                std::free(c_res->questions[i].bubbles);
            }
        }
        std::free(c_res->questions);
    }
    std::free(c_res);
}

} // namespace libreomr
