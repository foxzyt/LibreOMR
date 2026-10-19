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

#ifndef LIBREOMR_H
#define LIBREOMR_H

#include "libreomr/export.h"
#include "libreomr/types.h"
#include "libreomr/version.h"

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// Error Reporting
// ============================================================================

LIBREOMR_API const char* LIBREOMR_CALL libreomr_get_last_error(void);
LIBREOMR_API void LIBREOMR_CALL libreomr_clear_last_error(void);

// ============================================================================
// Image Management
// ============================================================================

LIBREOMR_API libreomr_status_t LIBREOMR_CALL libreomr_image_create(
    uint32_t width, uint32_t height, libreomr_image_format_t format, libreomr_image_t** out_img);

LIBREOMR_API libreomr_status_t LIBREOMR_CALL libreomr_image_create_from_buffer(
    uint32_t width, uint32_t height, libreomr_image_format_t format, const uint8_t* data, libreomr_image_t** out_img);

LIBREOMR_API libreomr_status_t LIBREOMR_CALL libreomr_image_load_file(
    const char* filepath, libreomr_image_t** out_img);

LIBREOMR_API libreomr_status_t LIBREOMR_CALL libreomr_image_save_file(
    const libreomr_image_t* img, const char* filepath);

LIBREOMR_API libreomr_status_t LIBREOMR_CALL libreomr_image_get_info(
    const libreomr_image_t* img, uint32_t* width, uint32_t* height, uint32_t* channels, size_t* stride);

LIBREOMR_API libreomr_status_t LIBREOMR_CALL libreomr_image_get_data(
    const libreomr_image_t* img, const uint8_t** out_data);

LIBREOMR_API void LIBREOMR_CALL libreomr_image_destroy(libreomr_image_t* img);

// ============================================================================
// Template Management
// ============================================================================

LIBREOMR_API libreomr_status_t LIBREOMR_CALL libreomr_template_create(
    const char* name, uint32_t width, uint32_t height, uint32_t dpi, libreomr_template_t** out_tmpl);

LIBREOMR_API libreomr_status_t LIBREOMR_CALL libreomr_template_load_json(
    const char* json_str, libreomr_template_t** out_tmpl);

LIBREOMR_API libreomr_status_t LIBREOMR_CALL libreomr_template_load_json_file(
    const char* filepath, libreomr_template_t** out_tmpl);

LIBREOMR_API libreomr_status_t LIBREOMR_CALL libreomr_template_save_json_file(
    const libreomr_template_t* tmpl, const char* filepath);

LIBREOMR_API libreomr_status_t LIBREOMR_CALL libreomr_template_set_fiducials(
    libreomr_template_t* tmpl, libreomr_quad_t fiducials);

LIBREOMR_API libreomr_status_t LIBREOMR_CALL libreomr_template_set_default_fiducials(
    libreomr_template_t* tmpl, float margin, float marker_size);

LIBREOMR_API libreomr_status_t LIBREOMR_CALL libreomr_template_add_question(
    libreomr_template_t* tmpl, const char* id, const char* name, libreomr_question_type_t type, float weight);

LIBREOMR_API libreomr_status_t LIBREOMR_CALL libreomr_template_add_bubble(
    libreomr_template_t* tmpl, const char* question_id, const char* label,
    float center_x, float center_y, float radius, libreomr_bubble_shape_t shape);

LIBREOMR_API libreomr_status_t LIBREOMR_CALL libreomr_template_set_answer_key(
    libreomr_template_t* tmpl, const char* question_id, const char* correct_answer);

LIBREOMR_API libreomr_status_t LIBREOMR_CALL libreomr_template_render_blank_sheet(
    const libreomr_template_t* tmpl, libreomr_image_t** out_sheet_img);

LIBREOMR_API void LIBREOMR_CALL libreomr_template_destroy(libreomr_template_t* tmpl);

// ============================================================================
// OMR Processor
// ============================================================================

LIBREOMR_API libreomr_status_t LIBREOMR_CALL libreomr_processor_create(libreomr_processor_t** out_proc);

LIBREOMR_API libreomr_status_t LIBREOMR_CALL libreomr_processor_set_fill_threshold(
    libreomr_processor_t* proc, float threshold);

LIBREOMR_API libreomr_status_t LIBREOMR_CALL libreomr_processor_set_auto_align(
    libreomr_processor_t* proc, bool enable);

LIBREOMR_API libreomr_status_t LIBREOMR_CALL libreomr_processor_process(
    libreomr_processor_t* proc, const libreomr_image_t* image,
    const libreomr_template_t* tmpl, libreomr_page_result_t** out_result);

LIBREOMR_API void LIBREOMR_CALL libreomr_processor_destroy(libreomr_processor_t* proc);

// ============================================================================
// Result Inspection & Export
// ============================================================================

LIBREOMR_API libreomr_status_t LIBREOMR_CALL libreomr_result_to_json(
    const libreomr_page_result_t* result, char** out_json_str);

LIBREOMR_API libreomr_status_t LIBREOMR_CALL libreomr_result_to_csv(
    const libreomr_page_result_t* result, char** out_csv_str);

LIBREOMR_API libreomr_status_t LIBREOMR_CALL libreomr_result_render_debug(
    const libreomr_page_result_t* result, const libreomr_image_t* source_image, libreomr_image_t** out_debug_img);

LIBREOMR_API void LIBREOMR_CALL libreomr_result_destroy(libreomr_page_result_t* result);

LIBREOMR_API void LIBREOMR_CALL libreomr_string_free(char* str);

#ifdef __cplusplus
}
#endif

#endif /* LIBREOMR_H */
