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

#ifndef LIBREOMR_TEMPLATE_HPP
#define LIBREOMR_TEMPLATE_HPP

#include "libreomr/export.h"
#include "libreomr/geometry.hpp"
#include "libreomr/image.hpp"
#include "libreomr/types.h"
#include <map>
#include <string>
#include <vector>

namespace libreomr {

struct LIBREOMR_API BubbleDefinition {
    std::string label;
    Point2Df center;
    float radius{12.0f};
    libreomr_bubble_shape_t shape{LIBREOMR_SHAPE_CIRCLE};

    [[nodiscard]] Rectf boundingBox() const {
        return {center.x - radius, center.y - radius, radius * 2.0f, radius * 2.0f};
    }
};

struct LIBREOMR_API QuestionDefinition {
    std::string id;
    std::string name;
    libreomr_question_type_t type{LIBREOMR_QUESTION_SINGLE_CHOICE};
    float weight{1.0f};
    std::string correct_answer;
    std::vector<BubbleDefinition> bubbles;

    void addBubble(const std::string& label, Point2Df center, float radius = 12.0f,
                   libreomr_bubble_shape_t shape = LIBREOMR_SHAPE_CIRCLE) {
        bubbles.push_back({label, center, radius, shape});
    }
};

class LIBREOMR_API Template {
public:
    Template();
    Template(std::string name, uint32_t width, uint32_t height, uint32_t dpi = 300);

    [[nodiscard]] const std::string& name() const noexcept { return name_; }
    void setName(const std::string& name) { name_ = name; }

    [[nodiscard]] uint32_t width() const noexcept { return width_; }
    [[nodiscard]] uint32_t height() const noexcept { return height_; }
    [[nodiscard]] uint32_t dpi() const noexcept { return dpi_; }

    void setDimensions(uint32_t width, uint32_t height, uint32_t dpi = 300) {
        width_ = width;
        height_ = height;
        dpi_ = dpi;
    }

    [[nodiscard]] const Quad2D& fiducials() const noexcept { return fiducials_; }
    void setFiducials(const Quad2D& quad) { fiducials_ = quad; }
    void setDefaultFiducials(float margin = 40.0f, float marker_size = 30.0f);

    [[nodiscard]] const std::vector<QuestionDefinition>& questions() const noexcept { return questions_; }
    std::vector<QuestionDefinition>& questions() noexcept { return questions_; }

    void addQuestion(const QuestionDefinition& question);
    [[nodiscard]] const QuestionDefinition* findQuestion(const std::string& id) const;
    QuestionDefinition* findQuestion(const std::string& id);

    void setAnswerKey(const std::string& question_id, const std::string& correct_answer);

    // Serialization
    [[nodiscard]] std::string toJson() const;
    bool saveToJsonFile(const std::string& filepath) const;

    static Template fromJson(const std::string& json_str);
    static Template loadFromJsonFile(const std::string& filepath);

    [[nodiscard]] Image renderBlankSheet() const;
    [[nodiscard]] Image renderFilledSheet(const std::map<std::string, std::string>& answers) const;

private:
    std::string name_{"Standard OMR Template"};
    uint32_t width_{1650};
    uint32_t height_{2330};
    uint32_t dpi_{300};
    Quad2D fiducials_{};
    std::vector<QuestionDefinition> questions_;
};

} // namespace libreomr

#endif /* LIBREOMR_TEMPLATE_HPP */
