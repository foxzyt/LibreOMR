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

TEST_CASE(TestProcessEmptyImageGracefulFallback) {
    Template tmpl("Empty Test", 800, 1100, 150);
    tmpl.setDefaultFiducials(40.0f, 24.0f);
    QuestionDefinition q;
    q.id = "q1";
    q.name = "Question 1";
    q.weight = 2.0f;
    q.addBubble("A", Point2Df(100, 100), 10.0f);
    tmpl.addQuestion(q);

    Processor processor;
    Image empty_image; // 0x0 empty image
    PageResult result = processor.process(empty_image, tmpl);

    ASSERT_FALSE(result.alignment_successful);
    ASSERT_EQ(result.questions.size(), 1u);
    ASSERT_FALSE(result.questions[0].is_answered);
    ASSERT_FALSE(result.questions[0].is_correct);
    ASSERT_EQ(result.questions[0].earned_score, 0.0f);
    ASSERT_EQ(result.questions[0].max_score, 2.0f);
    ASSERT_EQ(result.total_score, 0.0f);
    ASSERT_EQ(result.max_score, 2.0f);
    ASSERT_EQ(result.score_percentage, 0.0f);
}

TEST_CASE(TestRenderDebugImageWithCoordinateMapping) {
    uint32_t w = 800;
    uint32_t h = 1100;
    Template tmpl("Debug Test", w, h, 150);
    tmpl.setDefaultFiducials(40.0f, 24.0f);

    QuestionDefinition q;
    q.id = "q1";
    q.addBubble("A", Point2Df(200, 300), 12.0f);
    tmpl.addQuestion(q);

    std::map<std::string, std::string> ans = {{"q1", "A"}};
    Image sheet = tmpl.renderFilledSheet(ans);

    // Apply rotation
    Image skewed = sheet.rotate(2.0, 255);

    Processor processor;
    PageResult result = processor.process(skewed, tmpl);
    ASSERT_TRUE(result.alignment_successful);

    // Render debug image on skewed source image
    Image debug_overlay = result.renderDebugImage(skewed);
    ASSERT_EQ(debug_overlay.width(), skewed.width());
    ASSERT_EQ(debug_overlay.height(), skewed.height());
    ASSERT_EQ(debug_overlay.channels(), 3u); // converted to RGB overlay

    // Verify C API conversion memory safety
    libreomr_page_result_t* c_res = result.toC();
    ASSERT_TRUE(c_res != nullptr);
    ASSERT_TRUE(c_res->alignment_successful);
    ASSERT_TRUE(c_res->template_markers.top_left.x > 0.0f);
    PageResult::freeC(c_res);
}

TEST_CASE(TestBinarizationWithHarshShadow) {
    uint32_t w = 800;
    uint32_t h = 1000;
    Template tmpl("Shadow Test", w, h, 150);
    tmpl.setDefaultFiducials(40.0f, 24.0f);

    QuestionDefinition q1;
    q1.id = "q1";
    q1.addBubble("A", Point2Df(200, 300), 12.0f); // In shadowed area
    q1.addBubble("B", Point2Df(250, 300), 12.0f);
    tmpl.addQuestion(q1);

    QuestionDefinition q2;
    q2.id = "q2";
    q2.addBubble("A", Point2Df(550, 300), 12.0f); // In brightly lit area
    q2.addBubble("B", Point2Df(600, 300), 12.0f);
    tmpl.addQuestion(q2);

    std::map<std::string, std::string> ans = {{"q1", "A"}, {"q2", "B"}};
    Image sheet = tmpl.renderFilledSheet(ans);

    // Apply a harsh directional shadow across the left half of the page
    for (uint32_t y = 0; y < h; ++y) {
        for (uint32_t x = 0; x < w; ++x) {
            uint8_t val = sheet.at(x, y, 0);
            if (x < 400) {
                // Harsh shadow: drop intensity by 130 levels
                int darkened = std::max(15, static_cast<int>(val) - 130);
                sheet.at(x, y, 0) = static_cast<uint8_t>(darkened);
            }
        }
    }

    ProcessorConfig cfg;
    cfg.adaptive_thresholding = true;
    Processor proc(cfg);
    PageResult res = proc.process(sheet, tmpl);

    ASSERT_TRUE(res.alignment_successful);
    ASSERT_EQ(res.questions.size(), 2u);
    ASSERT_TRUE(res.questions[0].is_answered);
    ASSERT_EQ(res.questions[0].selected_answer, "A"); // Correctly read under heavy shadow
    ASSERT_TRUE(res.questions[1].is_answered);
    ASSERT_EQ(res.questions[1].selected_answer, "B"); // Correctly read in bright area
}

TEST_CASE(TestCurvedPaperCenterRefinement) {
    uint32_t w = 800;
    uint32_t h = 1000;
    Template tmpl("Curvature Test", w, h, 150);
    tmpl.setDefaultFiducials(40.0f, 24.0f);

    QuestionDefinition q;
    q.id = "q1";
    q.addBubble("A", Point2Df(300, 400), 14.0f);
    tmpl.addQuestion(q);

    // Render blank sheet then draw ink mark shifted by 5 pixels (simulating paper curl)
    Image sheet = tmpl.renderBlankSheet();
    sheet.drawFilledCircle(Point2Di(305, 403), 11, {10}); // Shifted ink

    ProcessorConfig cfg;
    cfg.refine_bubble_centers = true;
    Processor proc(cfg);
    PageResult res = proc.process(sheet, tmpl);

    ASSERT_TRUE(res.alignment_successful);
    ASSERT_TRUE(res.questions[0].is_answered);
    ASSERT_EQ(res.questions[0].selected_answer, "A");
    ASSERT_TRUE(res.questions[0].bubbles[0].is_marked);
}

TEST_CASE(TestAutoOrientationRotatedPage) {
    uint32_t w = 800;
    uint32_t h = 1100;
    Template tmpl("Orientation Test", w, h, 150);
    tmpl.setDefaultFiducials(40.0f, 24.0f);

    QuestionDefinition q;
    q.id = "q1";
    q.addBubble("A", Point2Df(200, 300), 12.0f);
    tmpl.addQuestion(q);

    std::map<std::string, std::string> ans = {{"q1", "A"}};
    Image sheet = tmpl.renderFilledSheet(ans);

    // Rotate by 90 degrees clockwise
    Image rotated90 = sheet.rotate(90.0, 255);

    ProcessorConfig cfg;
    cfg.auto_orient = true;
    Processor proc(cfg);

    PageResult res90 = proc.process(rotated90, tmpl);
    ASSERT_TRUE(res90.alignment_successful);
    ASSERT_TRUE(res90.questions[0].is_answered);
    ASSERT_EQ(res90.questions[0].selected_answer, "A");

    // Rotate by 180 degrees (upside down)
    Image rotated180 = sheet.rotate(180.0, 255);
    PageResult res180 = proc.process(rotated180, tmpl);
    ASSERT_TRUE(res180.alignment_successful);
    ASSERT_TRUE(res180.questions[0].is_answered);
    ASSERT_EQ(res180.questions[0].selected_answer, "A");

    // Rotate by 270 degrees
    Image rotated270 = sheet.rotate(270.0, 255);
    PageResult res270 = proc.process(rotated270, tmpl);
    ASSERT_TRUE(res270.alignment_successful);
    ASSERT_TRUE(res270.questions[0].is_answered);
    ASSERT_EQ(res270.questions[0].selected_answer, "A");
}

TEST_CASE(TestAutoOrientationBlankSheet) {
    uint32_t w = 800;
    uint32_t h = 1100;
    Template tmpl("Blank Orientation Test", w, h, 150);
    tmpl.setDefaultFiducials(40.0f, 24.0f);

    QuestionDefinition q;
    q.id = "q1";
    q.addBubble("A", Point2Df(200, 300), 12.0f);
    tmpl.addQuestion(q);

    // Render an unmarked blank sheet
    Image blank_sheet = tmpl.renderBlankSheet();

    // Rotate blank sheet by 180 degrees (upside down)
    Image rotated180 = blank_sheet.rotate(180.0, 255);

    ProcessorConfig cfg;
    cfg.auto_orient = true;
    Processor proc(cfg);
    PageResult res = proc.process(rotated180, tmpl);

    ASSERT_TRUE(res.alignment_successful);
    ASSERT_FALSE(res.questions[0].is_answered); // Unmarked blank sheet stays blank
}

TEST_CASE(TestNoCentroidDriftOnEmptyBubbleGlyph) {
    uint32_t w = 800;
    uint32_t h = 1000;
    Template tmpl("Glyph Drift Test", w, h, 150);
    tmpl.setDefaultFiducials(40.0f, 24.0f);

    QuestionDefinition q;
    q.id = "q1";
    q.addBubble("A", Point2Df(300, 400), 14.0f);
    tmpl.addQuestion(q);

    Image sheet = tmpl.renderBlankSheet();
    // Simulate printed letter 'A' inside bubble: small stroke of ~15 pixels shifted off-center
    for (int y = 402; y <= 405; ++y) {
        for (int x = 303; x <= 306; ++x) {
            sheet.at(x, y, 0) = 15;
        }
    }

    ProcessorConfig cfg;
    cfg.refine_bubble_centers = true;
    Processor proc(cfg);
    PageResult res = proc.process(sheet, tmpl);

    ASSERT_TRUE(res.alignment_successful);
    // Should NOT be marked as answered, and center should not shift
    ASSERT_FALSE(res.questions[0].is_answered);
    ASSERT_FALSE(res.questions[0].bubbles[0].is_marked);
    ASSERT_NEAR(res.questions[0].bubbles[0].center.x, 300.0f, 1.0f);
    ASSERT_NEAR(res.questions[0].bubbles[0].center.y, 400.0f, 1.0f);
}


