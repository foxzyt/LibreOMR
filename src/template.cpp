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
#include <algorithm>
#include <cctype>
#include <cmath>
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

Image Template::renderBlankSheet(bool include_decorations, const std::string& lang) const {
    Image sheet(width_, height_, 1);
    sheet.fill(255); // White background

    const std::vector<uint8_t> black = {0};
    const std::vector<uint8_t> dark_gray = {90};
    const std::vector<uint8_t> light_gray = {160};

    // 1. Draw 4 corner fiducial markers (solid black squares)
    const Point2Df corners[4] = {
        fiducials_.top_left, fiducials_.top_right, fiducials_.bottom_right, fiducials_.bottom_left
    };

    int marker_half = 16;
    if (dpi_ >= 250 && width_ >= 1000) {
        marker_half = std::max(16, static_cast<int>(width_ / 75)); // Proportional to resolution
    }

    for (const auto& pt : corners) {
        Recti r(static_cast<int>(pt.x) - marker_half,
                static_cast<int>(pt.y) - marker_half,
                marker_half * 2, marker_half * 2);
        sheet.drawFilledRect(r, black);
    }

    // 2. Draw Multilingual Header & Candidate Information Block (when decorations enabled on large sheets)
    if (include_decorations && width_ >= 900 && height_ >= 1000) {
        int scale_title = std::max(2, static_cast<int>(dpi_ / 75));
        int scale_sub = std::max(1, static_cast<int>(dpi_ / 130));
        int scale_text = std::max(1, static_cast<int>(dpi_ / 150));

        int left_x = static_cast<int>(fiducials_.top_left.x);
        int right_x = static_cast<int>(fiducials_.top_right.x);
        int top_y = static_cast<int>(fiducials_.top_left.y) + marker_half + 20;

        // Top Header Dividers
        sheet.drawLine(Point2Di(left_x, top_y), Point2Di(right_x, top_y), black, 3);
        sheet.drawLine(Point2Di(left_x, top_y + 4), Point2Di(right_x, top_y + 4), black, 1);

        std::string title;
        std::string subtitle;
        std::string line_name;
        std::string line_id;
        std::string directions;

        std::string l = lang;
        for (auto& c : l) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

        if (l.find("es") != std::string::npos) {
            title = name_.empty() ? "HOJA DE RESPUESTAS OFICIAL" : name_;
            subtitle = "PLANTILLA ESTANDARIZADA PARA LECTURA OPTICA (OMR)";
            line_name = "NOMBRE DEL ALUMNO: _______________________________________  FECHA: _______________";
            line_id   = "MATRICULA / ID:    _______________________________________  CURSO: _______________";
            directions = "INSTRUCCIONES: Rellene completamente los circulos con boligrafo oscuro. Una sola opcion.";
        } else if (l.find("en") != std::string::npos) {
            title = name_.empty() ? "OFFICIAL EXAMINATION ANSWER SHEET" : name_;
            subtitle = "OFFICIAL STANDARDIZED ASSESSMENT AND TESTING FORM";
            line_name = "STUDENT NAME: ___________________________________________  DATE: _______________";
            line_id   = "STUDENT ID:   ___________________________________________  ROOM: _______________";
            directions = "DIRECTIONS: Fill in bubbles completely with dark ink or pencil. Mark one choice per question.";
        } else { // default "pt"
            title = name_.empty() ? "FOLHA DE RESPOSTAS OFICIAL" : name_;
            subtitle = "CARTAO-RESPOSTA PADRONIZADO PARA LEITURA OPTICA AUTOMATIZADA";
            line_name = "NOME DO ALUNO: ___________________________________________  DATA: _______________";
            line_id   = "MATRICULA / N: ___________________________________________  TURMA: ______________";
            directions = "INSTRUCOES: Preencha totalmente os circulos com caneta preta ou azul. Uma opcao por questao.";
        }

        for (auto& c : title) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));

        int title_y = top_y + 16;
        sheet.drawText(Point2Di(left_x + 14, title_y), title, black, scale_title);
        sheet.drawText(Point2Di(left_x + 14, title_y + (8 * scale_title) + 6), subtitle, dark_gray, scale_sub);

        // Student Info Block
        int box_y = title_y + (8 * scale_title) + (8 * scale_sub) + 18;
        int box_h = (18 * scale_text) + 36;
        Recti info_box(left_x, box_y, right_x - left_x, box_h);
        sheet.drawRect(info_box, black, 2);

        sheet.drawText(Point2Di(info_box.x + 16, box_y + 12), line_name, black, scale_text);
        sheet.drawText(Point2Di(info_box.x + 16, box_y + 12 + (10 * scale_text) + 6), line_id, black, scale_text);

        // Instructions
        int inst_y = box_y + box_h + 14;
        sheet.drawText(Point2Di(left_x + 8, inst_y), directions, dark_gray, scale_text);

        int div_y = inst_y + (8 * scale_text) + 12;
        sheet.drawLine(Point2Di(left_x, div_y), Point2Di(right_x, div_y), black, 2);
    }

    // 3. Draw Questions, Numbers, and Bubbles
    int q_num = 1;
    for (const auto& q : questions_) {
        // Draw Question Number next to bubbles
        if (include_decorations && !q.bubbles.empty()) {
            const auto& first_b = q.bubbles.front();
            int q_scale = std::max(1, static_cast<int>(first_b.radius / 9.0f));
            std::string q_label = (q_num < 10 ? "0" : "") + std::to_string(q_num) + ".";
            int q_x = static_cast<int>(first_b.center.x - first_b.radius - (q_label.size() * 6 * q_scale) - 6);
            int q_y = static_cast<int>(first_b.center.y - (3.5f * q_scale));
            if (q_x > 0 && q_y > 0) {
                sheet.drawText(Point2Di(q_x, q_y), q_label, black, q_scale);
            }
        }

        for (const auto& b : q.bubbles) {
            Point2Di center(static_cast<int>(b.center.x), static_cast<int>(b.center.y));
            int r = static_cast<int>(b.radius);

            if (b.shape == LIBREOMR_SHAPE_CIRCLE) {
                sheet.drawCircle(center, r, black, 2);
            } else {
                Recti rect(center.x - r, center.y - r, r * 2, r * 2);
                sheet.drawRect(rect, black, 2);
            }

            // Draw letter label inside bubble if large enough
            if (include_decorations && !b.label.empty() && r >= 16) {
                int letter_scale = std::max(1, static_cast<int>(r / 10.0f));
                int lx = center.x - (3 * letter_scale);
                int ly = center.y - (3.5f * letter_scale);
                sheet.drawText(Point2Di(lx, ly), b.label, dark_gray, letter_scale);
            }
        }
        q_num++;
    }

    return sheet;
}

Image Template::renderFilledSheet(const std::map<std::string, std::string>& answers, bool include_decorations, const std::string& lang) const {
    Image sheet = renderBlankSheet(include_decorations, lang);
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

Template Template::createStandard(int num_questions, int num_options,
                                  const std::string& title,
                                  uint32_t dpi,
                                  const std::string& page_size,
                                  int columns) {
    if (num_questions <= 0) num_questions = 20;
    if (num_options < 2) num_options = 2;
    if (num_options > 10) num_options = 10;
    if (dpi < 72) dpi = 72;

    uint32_t sheet_w = 2480;
    uint32_t sheet_h = 3508;

    std::string norm_size = page_size;
    for (auto& c : norm_size) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

    if (norm_size == "letter" || norm_size == "usletter" || norm_size == "us-letter") {
        sheet_w = static_cast<uint32_t>(std::round(8.5f * dpi));
        sheet_h = static_cast<uint32_t>(std::round(11.0f * dpi));
    } else { // Default A4 (210 x 297 mm)
        sheet_w = static_cast<uint32_t>(std::round(210.0f / 25.4f * dpi));
        sheet_h = static_cast<uint32_t>(std::round(297.0f / 25.4f * dpi));
    }

    std::string template_name = title.empty() ? "Examination Answer Sheet" : title;
    Template tmpl(template_name, sheet_w, sheet_h, dpi);

    float margin = static_cast<float>(dpi) * 0.40f; // ~120px at 300 DPI
    float marker_size = static_cast<float>(dpi) * 0.20f; // ~60px at 300 DPI
    tmpl.setDefaultFiducials(margin, marker_size);

    int cols = columns;
    if (cols <= 0) {
        if (num_questions <= 25) cols = 1;
        else if (num_questions <= 50) cols = 2;
        else if (num_questions <= 100) cols = 4;
        else cols = 5;
    }
    cols = std::clamp(cols, 1, 8);

    int questions_per_col = (num_questions + cols - 1) / cols;

    float start_y = static_cast<float>(dpi) * 1.55f; // Safely below header and candidate info block
    float bottom_y = static_cast<float>(sheet_h) - margin - (static_cast<float>(dpi) * 0.25f);
    float available_h = bottom_y - start_y;

    float row_h = available_h / static_cast<float>(questions_per_col);
    float max_row_h = static_cast<float>(dpi) * 0.26f;
    if (row_h > max_row_h) row_h = max_row_h;

    float bubble_radius = std::min(row_h * 0.26f, static_cast<float>(dpi) * 0.058f);
    bubble_radius = std::max(7.0f, bubble_radius);

    float bubble_spacing = bubble_radius * 2.85f;
    float col_span = (static_cast<float>(sheet_w) - (margin * 2.0f)) / static_cast<float>(cols);

    float label_pad = bubble_radius * 3.6f;
    float block_width = label_pad + (num_options * bubble_spacing);

    for (int q = 1; q <= num_questions; ++q) {
        QuestionDefinition q_def;
        q_def.id = "q" + std::to_string(q);
        q_def.name = "Question " + std::to_string(q);
        q_def.type = LIBREOMR_QUESTION_SINGLE_CHOICE;
        q_def.weight = 1.0f;

        // Default cyclic answer key A, B, C, D, E...
        std::string key(1, static_cast<char>('A' + ((q - 1) % num_options)));
        q_def.correct_answer = key;

        int c = (q - 1) / questions_per_col;
        int r = (q - 1) % questions_per_col;

        float col_left = margin + (c * col_span);
        float offset_x = (col_span - block_width) * 0.5f;
        if (offset_x < label_pad) offset_x = label_pad;

        float base_x = col_left + offset_x + (bubble_radius * 1.2f);
        float base_y = start_y + (r * row_h) + (row_h * 0.5f);

        for (int opt = 0; opt < num_options; ++opt) {
            std::string label(1, static_cast<char>('A' + opt));
            float bx = base_x + (opt * bubble_spacing);
            q_def.addBubble(label, Point2Df(bx, base_y), bubble_radius);
        }

        tmpl.addQuestion(q_def);
    }

    return tmpl;
}

} // namespace libreomr
