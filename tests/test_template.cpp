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

#include "test_framework.hpp"
#include "libreomr/template.hpp"

using namespace libreomr;

TEST_CASE(TestTemplateCreationAndSerialization) {
    Template tmpl("Exam Template", 1000, 1400, 300);
    tmpl.setDefaultFiducials(40.0f, 24.0f);

    QuestionDefinition q1;
    q1.id = "q1";
    q1.name = "Question 1";
    q1.type = LIBREOMR_QUESTION_SINGLE_CHOICE;
    q1.weight = 2.0f;
    q1.correct_answer = "C";
    q1.addBubble("A", Point2Df(100, 200), 12.0f);
    q1.addBubble("B", Point2Df(140, 200), 12.0f);
    q1.addBubble("C", Point2Df(180, 200), 12.0f);
    q1.addBubble("D", Point2Df(220, 200), 12.0f);

    tmpl.addQuestion(q1);

    std::string json_str = tmpl.toJson();
    ASSERT_FALSE(json_str.empty());

    Template loaded = Template::fromJson(json_str);
    ASSERT_EQ(loaded.name(), "Exam Template");
    ASSERT_EQ(loaded.width(), 1000u);
    ASSERT_EQ(loaded.height(), 1400u);
    ASSERT_EQ(loaded.questions().size(), 1u);

    const auto* loaded_q1 = loaded.findQuestion("q1");
    ASSERT_TRUE(loaded_q1 != nullptr);
    ASSERT_EQ(loaded_q1->correct_answer, "C");
    ASSERT_EQ(loaded_q1->weight, 2.0f);
    ASSERT_EQ(loaded_q1->bubbles.size(), 4u);
    ASSERT_EQ(loaded_q1->bubbles[2].label, "C");
}

TEST_CASE(TestBlankSheetRendering) {
    Template tmpl("Test Sheet", 600, 800, 150);
    tmpl.setDefaultFiducials(30.0f, 20.0f);

    QuestionDefinition q;
    q.id = "q1";
    q.addBubble("A", Point2Df(100, 150), 10.0f);
    q.addBubble("B", Point2Df(150, 150), 10.0f);
    tmpl.addQuestion(q);

    Image sheet = tmpl.renderBlankSheet();
    ASSERT_EQ(sheet.width(), 600u);
    ASSERT_EQ(sheet.height(), 800u);

    // Center of bubble A should be white (255) inside
    ASSERT_EQ(sheet.at(100, 150, 0), 255);
}
