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

#ifndef LIBREOMR_TYPES_H
#define LIBREOMR_TYPES_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum libreomr_status_t {
    LIBREOMR_SUCCESS = 0,
    LIBREOMR_ERROR_INVALID_ARGUMENT = -1,
    LIBREOMR_ERROR_FILE_IO = -2,
    LIBREOMR_ERROR_OUT_OF_MEMORY = -3,
    LIBREOMR_ERROR_PARSING = -4,
    LIBREOMR_ERROR_ALIGNMENT_FAILED = -5,
    LIBREOMR_ERROR_PROCESSING = -6,
    LIBREOMR_ERROR_NOT_FOUND = -7,
    LIBREOMR_ERROR_INTERNAL = -99
} libreomr_status_t;

typedef enum libreomr_image_format_t {
    LIBREOMR_IMAGE_GRAY8 = 1,
    LIBREOMR_IMAGE_RGB8 = 3,
    LIBREOMR_IMAGE_RGBA8 = 4
} libreomr_image_format_t;

typedef enum libreomr_bubble_shape_t {
    LIBREOMR_SHAPE_CIRCLE = 0,
    LIBREOMR_SHAPE_RECTANGLE = 1
} libreomr_bubble_shape_t;

typedef enum libreomr_question_type_t {
    LIBREOMR_QUESTION_SINGLE_CHOICE = 0,
    LIBREOMR_QUESTION_MULTI_CHOICE = 1
} libreomr_question_type_t;

typedef struct libreomr_point_t {
    float x;
    float y;
} libreomr_point_t;

typedef struct libreomr_rect_t {
    float x;
    float y;
    float width;
    float height;
} libreomr_rect_t;

typedef struct libreomr_quad_t {
    libreomr_point_t top_left;
    libreomr_point_t top_right;
    libreomr_point_t bottom_right;
    libreomr_point_t bottom_left;
} libreomr_quad_t;

typedef struct libreomr_bubble_metric_t {
    char label[32];
    float fill_ratio;
    float mean_intensity;
    float relative_fill;
    bool is_marked;
    libreomr_rect_t bounds;
} libreomr_bubble_metric_t;

typedef struct libreomr_question_result_t {
    char question_id[64];
    char question_name[64];
    uint32_t num_bubbles;
    libreomr_bubble_metric_t* bubbles;
    char selected_answer[64];
    float confidence;
    bool is_answered;
    bool is_ambiguous;
    bool is_correct;
    float earned_score;
    float max_score;
} libreomr_question_result_t;

typedef struct libreomr_page_result_t {
    bool alignment_successful;
    libreomr_quad_t detected_markers;
    float alignment_skew_degrees;
    uint32_t num_questions;
    libreomr_question_result_t* questions;
    float total_score;
    float max_score;
    float score_percentage;
} libreomr_page_result_t;

typedef struct libreomr_image_t libreomr_image_t;
typedef struct libreomr_template_t libreomr_template_t;
typedef struct libreomr_processor_t libreomr_processor_t;

#ifdef __cplusplus
}
#endif

#endif /* LIBREOMR_TYPES_H */
