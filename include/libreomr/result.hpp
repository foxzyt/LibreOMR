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

#ifndef LIBREOMR_RESULT_HPP
#define LIBREOMR_RESULT_HPP

#include "libreomr/export.h"
#include "libreomr/geometry.hpp"
#include "libreomr/image.hpp"
#include "libreomr/types.h"
#include <string>
#include <vector>

namespace libreomr {

struct LIBREOMR_API BubbleMetric {
    std::string label;
    Point2Df center;
    Rectf bounds;
    float fill_ratio{0.0f};
    float mean_intensity{255.0f};
    float local_background{255.0f};
    float relative_fill{0.0f};
    bool is_marked{false};
};

struct LIBREOMR_API QuestionResult {
    std::string question_id;
    std::string question_name;
    libreomr_question_type_t type{LIBREOMR_QUESTION_SINGLE_CHOICE};
    std::vector<BubbleMetric> bubbles;
    std::string selected_answer;
    float confidence{0.0f};
    bool is_answered{false};
    bool is_ambiguous{false};
    bool is_correct{false};
    float earned_score{0.0f};
    float max_score{1.0f};
};

class LIBREOMR_API PageResult {
public:
    bool alignment_successful{false};
    float alignment_skew_degrees{0.0f};
    Quad2D detected_fiducials;
    std::vector<QuestionResult> questions;
    float total_score{0.0f};
    float max_score{0.0f};
    float score_percentage{0.0f};

    [[nodiscard]] std::string toJson() const;
    [[nodiscard]] std::string toCsv() const;
    bool saveToJsonFile(const std::string& filepath) const;
    bool saveToCsvFile(const std::string& filepath) const;

    [[nodiscard]] Image renderDebugImage(const Image& base_image) const;

    [[nodiscard]] libreomr_page_result_t* toC() const;
    static void freeC(libreomr_page_result_t* c_res);
};

} // namespace libreomr

#endif /* LIBREOMR_RESULT_HPP */
