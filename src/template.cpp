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

#include "libreomr/template.hpp"
#include "libreomr/json.hpp"
#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace libreomr {

Template::Template() {
    setDefaultFiducials();
}

Template::Template(std::string name, uint32_t width, uint32_t height, uint32_t dpi)
    : name_(std::move(name)), width_(width), height_(height), dpi_(dpi) {
    setDefaultFiducials();
}

void Template::setDefaultFiducials(float margin, float marker_size) {
    float half = marker_size / 2.0f;
    float left = margin + half;
    float top = margin + half;
    float right = static_cast<float>(width_) - margin - half;
    float bottom = static_cast<float>(height_) - margin - half;

    fiducials_ = Quad2D(
        Point2Df(left, top),
        Point2Df(right, top),
        Point2Df(right, bottom),
        Point2Df(left, bottom)
    );
}

void Template::addQuestion(const QuestionDefinition& question) {
    questions_.push_back(question);
}

const QuestionDefinition* Template::findQuestion(const std::string& id) const {
    for (const auto& q : questions_) {
        if (q.id == id) return &q;
    }
    return nullptr;
}

QuestionDefinition* Template::findQuestion(const std::string& id) {
    for (auto& q : questions_) {
        if (q.id == id) return &q;
    }
    return nullptr;
}

void Template::setAnswerKey(const std::string& question_id, const std::string& correct_answer) {
    auto* q = findQuestion(question_id);
    if (q) {
        q->correct_answer = correct_answer;
    }
}

std::string Template::toJson() const {
    JsonValue root(JsonType::Object);
    root.set("name", JsonValue(name_));
    root.set("width", JsonValue(static_cast<int>(width_)));
    root.set("height", JsonValue(static_cast<int>(height_)));
    root.set("dpi", JsonValue(static_cast<int>(dpi_)));

    // Fiducials
    JsonValue fids(JsonType::Object);
    auto makePt = [](const Point2Df& pt) {
        JsonValue p(JsonType::Object);
        p.set("x", JsonValue(pt.x));
        p.set("y", JsonValue(pt.y));
        return p;
    };
    fids.set("top_left", makePt(fiducials_.top_left));
    fids.set("top_right", makePt(fiducials_.top_right));
    fids.set("bottom_right", makePt(fiducials_.bottom_right));
    fids.set("bottom_left", makePt(fiducials_.bottom_left));
    root.set("fiducials", fids);

    // Questions
    JsonValue q_arr(JsonType::Array);
    for (const auto& q : questions_) {
        JsonValue q_obj(JsonType::Object);
        q_obj.set("id", JsonValue(q.id));
        q_obj.set("name", JsonValue(q.name));
        q_obj.set("type", JsonValue(static_cast<int>(q.type)));
        q_obj.set("weight", JsonValue(q.weight));
        q_obj.set("correct_answer", JsonValue(q.correct_answer));

        JsonValue b_arr(JsonType::Array);
        for (const auto& b : q.bubbles) {
            JsonValue b_obj(JsonType::Object);
            b_obj.set("label", JsonValue(b.label));
            b_obj.set("center_x", JsonValue(b.center.x));
            b_obj.set("center_y", JsonValue(b.center.y));
            b_obj.set("radius", JsonValue(b.radius));
            b_obj.set("shape", JsonValue(static_cast<int>(b.shape)));
            b_arr.pushBack(b_obj);
        }
        q_obj.set("bubbles", b_arr);
        q_arr.pushBack(q_obj);
    }
    root.set("questions", q_arr);

    return root.serialize(2);
}

bool Template::saveToJsonFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) return false;
    file << toJson();
    return file.good();
}

Template Template::fromJson(const std::string& json_str) {
    std::string err;
    JsonValue root = JsonValue::parse(json_str, &err);
    if (!err.empty() || !root.isObject()) {
        throw std::runtime_error("Invalid JSON template: " + err);
    }

    std::string name = root.get("name").asString();
    uint32_t width = root.get("width").asUint(1650);
    uint32_t height = root.get("height").asUint(2330);
    uint32_t dpi = root.get("dpi").asUint(300);

    Template tmpl(name, width, height, dpi);

    if (root.contains("fiducials")) {
        const auto& fids = root.get("fiducials");
        auto parsePt = [](const JsonValue& v) -> Point2Df {
            return {static_cast<float>(v.get("x").asDouble(0.0)),
                    static_cast<float>(v.get("y").asDouble(0.0))};
        };
        Quad2D q;
        q.top_left = parsePt(fids.get("top_left"));
        q.top_right = parsePt(fids.get("top_right"));
        q.bottom_right = parsePt(fids.get("bottom_right"));
        q.bottom_left = parsePt(fids.get("bottom_left"));
        tmpl.setFiducials(q);
    }

    if (root.contains("questions")) {
        for (const auto& q_val : root.get("questions").arrayValues()) {
            QuestionDefinition q;
            q.id = q_val.get("id").asString();
            q.name = q_val.get("name").asString();
            q.type = static_cast<libreomr_question_type_t>(q_val.get("type").asInt(0));
            q.weight = static_cast<float>(q_val.get("weight").asDouble(1.0));
            q.correct_answer = q_val.get("correct_answer").asString();

            if (q_val.contains("bubbles")) {
                for (const auto& b_val : q_val.get("bubbles").arrayValues()) {
                    BubbleDefinition b;
                    b.label = b_val.get("label").asString();
                    b.center.x = static_cast<float>(b_val.get("center_x").asDouble(0.0));
                    b.center.y = static_cast<float>(b_val.get("center_y").asDouble(0.0));
                    b.radius = static_cast<float>(b_val.get("radius").asDouble(12.0));
                    b.shape = static_cast<libreomr_bubble_shape_t>(b_val.get("shape").asInt(0));
                    q.bubbles.push_back(b);
                }
            }
            tmpl.addQuestion(q);
        }
    }

    return tmpl;
}

Template Template::loadFromJsonFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open template file: " + filepath);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return fromJson(buffer.str());
}

Image Template::renderBlankSheet() const {
    Image sheet(width_, height_, 1);
    sheet.fill(255); // White background

    const std::vector<uint8_t> black = {0};

    // Draw 4 corner fiducial markers (solid black squares, 32x32)
    const Point2Df corners[4] = {
        fiducials_.top_left, fiducials_.top_right, fiducials_.bottom_right, fiducials_.bottom_left
    };

    const int marker_half = 16;
    for (const auto& pt : corners) {
        Recti r(static_cast<int>(pt.x) - marker_half,
                static_cast<int>(pt.y) - marker_half,
                marker_half * 2, marker_half * 2);
        sheet.drawFilledRect(r, black);
    }

    // Draw bubbles
    for (const auto& q : questions_) {
        for (const auto& b : q.bubbles) {
            Point2Di center(static_cast<int>(b.center.x), static_cast<int>(b.center.y));
            int r = static_cast<int>(b.radius);

            if (b.shape == LIBREOMR_SHAPE_CIRCLE) {
                // Circle border with 2-pixel thickness
                sheet.drawCircle(center, r, black, 2);
            } else {
                Recti rect(center.x - r, center.y - r, r * 2, r * 2);
                sheet.drawRect(rect, black, 2);
            }
        }
    }

    return sheet;
}

Image Template::renderFilledSheet(const std::map<std::string, std::string>& answers) const {
    Image sheet = renderBlankSheet();
    const std::vector<uint8_t> fill_color = {25}; // Dark pencil/pen simulation

    for (const auto& q : questions_) {
        auto it = answers.find(q.id);
        if (it == answers.end()) continue;

        const std::string& target_ans = it->second;

        for (const auto& b : q.bubbles) {
            // Check if label matches or is part of comma/semicolon-separated answer tokens
            bool should_fill = false;
            if (!b.label.empty()) {
                if (target_ans == b.label) {
                    should_fill = true;
                } else {
                    size_t start = 0;
                    while (start < target_ans.size()) {
                        size_t end = target_ans.find_first_of(",;", start);
                        if (end == std::string::npos) end = target_ans.size();

                        size_t token_start = start;
                        while (token_start < end && std::isspace(static_cast<unsigned char>(target_ans[token_start]))) {
                            token_start++;
                        }
                        size_t token_end = end;
                        while (token_end > token_start && std::isspace(static_cast<unsigned char>(target_ans[token_end - 1]))) {
                            token_end--;
                        }

                        if (token_end > token_start) {
                            if (target_ans.compare(token_start, token_end - token_start, b.label) == 0) {
                                should_fill = true;
                                break;
                            }
                        }

                        start = end + 1;
                    }
                }
            }

            if (should_fill) {
                Point2Di center(static_cast<int>(b.center.x), static_cast<int>(b.center.y));
                int fill_radius = static_cast<int>(b.radius * 0.85f);
                if (b.shape == LIBREOMR_SHAPE_CIRCLE) {
                    sheet.drawFilledCircle(center, fill_radius, fill_color);
                } else {
                    Recti r(center.x - fill_radius, center.y - fill_radius, fill_radius * 2, fill_radius * 2);
                    sheet.drawFilledRect(r, fill_color);
                }
            }
        }
    }

    return sheet;
}

} // namespace libreomr
