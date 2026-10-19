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
#include "libreomr/libreomr.h"
#include <cstring>

TEST_CASE(TestCApiVersion) {
    ASSERT_EQ(libreomr_version_major(), 1);
    ASSERT_EQ(libreomr_version_minor(), 0);
    ASSERT_EQ(libreomr_version_patch(), 0);
    ASSERT_TRUE(std::strcmp(libreomr_version_string(), "1.0.0") == 0);
}

TEST_CASE(TestCApiImageLifecycle) {
    libreomr_image_t* img = nullptr;
    libreomr_status_t status = libreomr_image_create(640, 480, LIBREOMR_IMAGE_GRAY8, &img);
    ASSERT_EQ(status, LIBREOMR_SUCCESS);
    ASSERT_TRUE(img != nullptr);

    uint32_t w = 0, h = 0, ch = 0;
    size_t stride = 0;
    status = libreomr_image_get_info(img, &w, &h, &ch, &stride);
    ASSERT_EQ(status, LIBREOMR_SUCCESS);
    ASSERT_EQ(w, 640u);
    ASSERT_EQ(h, 480u);
    ASSERT_EQ(ch, 1u);
    ASSERT_EQ(stride, 640u);

    const uint8_t* data = nullptr;
    status = libreomr_image_get_data(img, &data);
    ASSERT_EQ(status, LIBREOMR_SUCCESS);
    ASSERT_TRUE(data != nullptr);

    libreomr_image_destroy(img);
}

TEST_CASE(TestCApiFullPipeline) {
    // 1. Create Template via C API
    libreomr_template_t* tmpl = nullptr;
    libreomr_status_t status = libreomr_template_create("C API Test", 800, 1000, 150, &tmpl);
    ASSERT_EQ(status, LIBREOMR_SUCCESS);
    ASSERT_TRUE(tmpl != nullptr);

    status = libreomr_template_set_default_fiducials(tmpl, 30.0f, 20.0f);
    ASSERT_EQ(status, LIBREOMR_SUCCESS);

    status = libreomr_template_add_question(tmpl, "q1", "Question 1", LIBREOMR_QUESTION_SINGLE_CHOICE, 1.0f);
    ASSERT_EQ(status, LIBREOMR_SUCCESS);

    status = libreomr_template_add_bubble(tmpl, "q1", "A", 100.0f, 200.0f, 12.0f, LIBREOMR_SHAPE_CIRCLE);
    ASSERT_EQ(status, LIBREOMR_SUCCESS);
    status = libreomr_template_add_bubble(tmpl, "q1", "B", 150.0f, 200.0f, 12.0f, LIBREOMR_SHAPE_CIRCLE);
    ASSERT_EQ(status, LIBREOMR_SUCCESS);

    status = libreomr_template_set_answer_key(tmpl, "q1", "B");
    ASSERT_EQ(status, LIBREOMR_SUCCESS);

    // 2. Render blank sheet through C API
    libreomr_image_t* sheet_img = nullptr;
    status = libreomr_template_render_blank_sheet(tmpl, &sheet_img);
    ASSERT_EQ(status, LIBREOMR_SUCCESS);
    ASSERT_TRUE(sheet_img != nullptr);

    // 3. Create Processor
    libreomr_processor_t* proc = nullptr;
    status = libreomr_processor_create(&proc);
    ASSERT_EQ(status, LIBREOMR_SUCCESS);
    ASSERT_TRUE(proc != nullptr);

    libreomr_processor_set_fill_threshold(proc, 0.30f);
    libreomr_processor_set_auto_align(proc, true);

    // 4. Process
    libreomr_page_result_t* result = nullptr;
    status = libreomr_processor_process(proc, sheet_img, tmpl, &result);
    ASSERT_EQ(status, LIBREOMR_SUCCESS);
    ASSERT_TRUE(result != nullptr);
    ASSERT_EQ(result->num_questions, 1u);
    // Unmarked blank sheet -> is_answered = false
    ASSERT_FALSE(result->questions[0].is_answered);

    // 5. Test JSON and CSV export
    char* json_out = nullptr;
    status = libreomr_result_to_json(result, &json_out);
    ASSERT_EQ(status, LIBREOMR_SUCCESS);
    ASSERT_TRUE(json_out != nullptr);
    libreomr_string_free(json_out);

    char* csv_out = nullptr;
    status = libreomr_result_to_csv(result, &csv_out);
    ASSERT_EQ(status, LIBREOMR_SUCCESS);
    ASSERT_TRUE(csv_out != nullptr);
    libreomr_string_free(csv_out);

    // 6. Cleanup
    libreomr_result_destroy(result);
    libreomr_processor_destroy(proc);
    libreomr_image_destroy(sheet_img);
    libreomr_template_destroy(tmpl);
}
