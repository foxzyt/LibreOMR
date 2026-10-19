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
#include "libreomr/libreomr.hpp"

using namespace libreomr;

TEST_CASE(TestEndToEndOMRPipelineDirect) {
    uint32_t w = 800;
    uint32_t h = 1100;
    Template tmpl("10-Question Test", w, h, 150);
    tmpl.setDefaultFiducials(40.0f, 24.0f);

    std::map<std::string, std::string> student_answers = {
        {"q1", "B"}, {"q2", "D"}, {"q3", "A"}, {"q4", "C"}, {"q5", "E"},
        {"q6", "A"}, {"q7", "B"}, {"q8", "D"}, {"q9", "C"}, {"q10", "B"}
    };

    float start_y = 150.0f;
    float row_h = 80.0f;
    float base_x = 180.0f;
    float bubble_spacing = 40.0f;
    float radius = 12.0f;

    for (int q = 1; q <= 10; ++q) {
        QuestionDefinition q_def;
        q_def.id = "q" + std::to_string(q);
        q_def.name = "Question " + std::to_string(q);
        q_def.type = LIBREOMR_QUESTION_SINGLE_CHOICE;
        q_def.weight = 1.0f;
        q_def.correct_answer = student_answers[q_def.id]; // 100% correct student

        float y = start_y + (q - 1) * row_h;
        for (int opt = 0; opt < 5; ++opt) {
            std::string label(1, static_cast<char>('A' + opt));
            float x = base_x + opt * bubble_spacing;
            q_def.addBubble(label, Point2Df(x, y), radius);
        }
        tmpl.addQuestion(q_def);
    }

    // Render filled answer sheet
    Image filled_sheet = tmpl.renderFilledSheet(student_answers);

    Processor processor;
    PageResult result = processor.process(filled_sheet, tmpl);

    ASSERT_EQ(result.questions.size(), 10u);
    for (const auto& q_res : result.questions) {
        ASSERT_TRUE(q_res.is_answered);
        ASSERT_FALSE(q_res.is_ambiguous);
        ASSERT_EQ(q_res.selected_answer, student_answers[q_res.question_id]);
        ASSERT_TRUE(q_res.is_correct);
        ASSERT_TRUE(q_res.confidence > 0.40f);
    }

    ASSERT_EQ(result.total_score, 10.0f);
    ASSERT_EQ(result.max_score, 10.0f);
    ASSERT_EQ(result.score_percentage, 100.0f);
}

TEST_CASE(TestEndToEndOMRPipelineWithPerspectiveAndSkew) {
    uint32_t w = 800;
    uint32_t h = 1100;
    Template tmpl("Skewed Sheet Test", w, h, 150);
    tmpl.setDefaultFiducials(50.0f, 28.0f);

    std::map<std::string, std::string> student_answers = {
        {"q1", "A"}, {"q2", "C"}, {"q3", "E"}
    };

    for (int q = 1; q <= 3; ++q) {
        QuestionDefinition q_def;
        q_def.id = "q" + std::to_string(q);
        q_def.type = LIBREOMR_QUESTION_SINGLE_CHOICE;
        q_def.weight = 2.0f;
        q_def.correct_answer = student_answers[q_def.id];

        float y = 200.0f + (q - 1) * 100.0f;
        for (int opt = 0; opt < 5; ++opt) {
            std::string label(1, static_cast<char>('A' + opt));
            float x = 200.0f + opt * 50.0f;
            q_def.addBubble(label, Point2Df(x, y), 14.0f);
        }
        tmpl.addQuestion(q_def);
    }

    Image perfect_sheet = tmpl.renderFilledSheet(student_answers);

    // Apply a rotation of 1.5 degrees to simulate skewed scanner feed
    Image skewed_sheet = perfect_sheet.rotate(1.5, 255);

    Processor processor;
    PageResult result = processor.process(skewed_sheet, tmpl);

    ASSERT_TRUE(result.alignment_successful);
    ASSERT_NEAR(result.alignment_skew_degrees, 1.5f, 0.6f);

    for (const auto& q_res : result.questions) {
        ASSERT_TRUE(q_res.is_answered);
        ASSERT_FALSE(q_res.is_ambiguous);
        ASSERT_EQ(q_res.selected_answer, student_answers[q_res.question_id]);
        ASSERT_TRUE(q_res.is_correct);
    }

    ASSERT_EQ(result.total_score, 6.0f);
}
