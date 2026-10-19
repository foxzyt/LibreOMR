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

#include "libreomr/libreomr.h"
#include "libreomr/libreomr.hpp"
#include <cstdlib>
#include <cstring>
#include <string>

struct libreomr_image_t {
    libreomr::Image impl;
};

struct libreomr_template_t {
    libreomr::Template impl;
};

struct libreomr_processor_t {
    libreomr::Processor impl;
};

namespace {
thread_local std::string g_last_error;

void setLastError(const std::string& err) {
    g_last_error = err;
}
} // namespace

extern "C" {

const char* libreomr_get_last_error(void) {
    return g_last_error.c_str();
}

void libreomr_clear_last_error(void) {
    g_last_error.clear();
}

libreomr_status_t libreomr_image_create(
    uint32_t width, uint32_t height, libreomr_image_format_t format, libreomr_image_t** out_img) {
    if (!out_img || width == 0 || height == 0) {
        setLastError("Invalid arguments to libreomr_image_create");
        return LIBREOMR_ERROR_INVALID_ARGUMENT;
    }
    try {
        auto* wrapper = new libreomr_image_t();
        wrapper->impl = libreomr::Image(width, height, static_cast<uint32_t>(format));
        *out_img = wrapper;
        return LIBREOMR_SUCCESS;
    } catch (const std::exception& ex) {
        setLastError(ex.what());
        return LIBREOMR_ERROR_OUT_OF_MEMORY;
    }
}

libreomr_status_t libreomr_image_create_from_buffer(
    uint32_t width, uint32_t height, libreomr_image_format_t format, const uint8_t* data, libreomr_image_t** out_img) {
    if (!out_img || !data || width == 0 || height == 0) {
        setLastError("Invalid arguments to libreomr_image_create_from_buffer");
        return LIBREOMR_ERROR_INVALID_ARGUMENT;
    }
    try {
        auto* wrapper = new libreomr_image_t();
        wrapper->impl = libreomr::Image(width, height, static_cast<uint32_t>(format), data);
        *out_img = wrapper;
        return LIBREOMR_SUCCESS;
    } catch (const std::exception& ex) {
        setLastError(ex.what());
        return LIBREOMR_ERROR_OUT_OF_MEMORY;
    }
}

libreomr_status_t libreomr_image_load_file(const char* filepath, libreomr_image_t** out_img) {
    if (!filepath || !out_img) {
        setLastError("Null pointer passed to libreomr_image_load_file");
        return LIBREOMR_ERROR_INVALID_ARGUMENT;
    }
    try {
        auto* wrapper = new libreomr_image_t();
        wrapper->impl = libreomr::Image::loadFromFile(filepath);
        *out_img = wrapper;
        return LIBREOMR_SUCCESS;
    } catch (const std::exception& ex) {
        setLastError(ex.what());
        return LIBREOMR_ERROR_FILE_IO;
    }
}

libreomr_status_t libreomr_image_save_file(const libreomr_image_t* img, const char* filepath) {
    if (!img || !filepath) {
        setLastError("Null pointer passed to libreomr_image_save_file");
        return LIBREOMR_ERROR_INVALID_ARGUMENT;
    }
    try {
        if (!img->impl.saveToFile(filepath)) {
            setLastError("Failed to write image file");
            return LIBREOMR_ERROR_FILE_IO;
        }
        return LIBREOMR_SUCCESS;
    } catch (const std::exception& ex) {
        setLastError(ex.what());
        return LIBREOMR_ERROR_FILE_IO;
    }
}

libreomr_status_t libreomr_image_get_info(
    const libreomr_image_t* img, uint32_t* width, uint32_t* height, uint32_t* channels, size_t* stride) {
    if (!img) return LIBREOMR_ERROR_INVALID_ARGUMENT;
    if (width) *width = img->impl.width();
    if (height) *height = img->impl.height();
    if (channels) *channels = img->impl.channels();
    if (stride) *stride = img->impl.stride();
    return LIBREOMR_SUCCESS;
}

libreomr_status_t libreomr_image_get_data(const libreomr_image_t* img, const uint8_t** out_data) {
    if (!img || !out_data) return LIBREOMR_ERROR_INVALID_ARGUMENT;
    *out_data = img->impl.data();
    return LIBREOMR_SUCCESS;
}

void libreomr_image_destroy(libreomr_image_t* img) {
    delete img;
}

libreomr_status_t libreomr_template_create(
    const char* name, uint32_t width, uint32_t height, uint32_t dpi, libreomr_template_t** out_tmpl) {
    if (!out_tmpl || width == 0 || height == 0) {
        setLastError("Invalid arguments to libreomr_template_create");
        return LIBREOMR_ERROR_INVALID_ARGUMENT;
    }
    try {
        auto* wrapper = new libreomr_template_t();
        wrapper->impl = libreomr::Template(name ? name : "OMR Template", width, height, dpi);
        *out_tmpl = wrapper;
        return LIBREOMR_SUCCESS;
    } catch (const std::exception& ex) {
        setLastError(ex.what());
        return LIBREOMR_ERROR_OUT_OF_MEMORY;
    }
}

libreomr_status_t libreomr_template_load_json(const char* json_str, libreomr_template_t** out_tmpl) {
    if (!json_str || !out_tmpl) {
        setLastError("Null pointer passed to libreomr_template_load_json");
        return LIBREOMR_ERROR_INVALID_ARGUMENT;
    }
    try {
        auto* wrapper = new libreomr_template_t();
        wrapper->impl = libreomr::Template::fromJson(json_str);
        *out_tmpl = wrapper;
        return LIBREOMR_SUCCESS;
    } catch (const std::exception& ex) {
        setLastError(ex.what());
        return LIBREOMR_ERROR_PARSING;
    }
}

libreomr_status_t libreomr_template_load_json_file(const char* filepath, libreomr_template_t** out_tmpl) {
    if (!filepath || !out_tmpl) {
        setLastError("Null pointer passed to libreomr_template_load_json_file");
        return LIBREOMR_ERROR_INVALID_ARGUMENT;
    }
    try {
        auto* wrapper = new libreomr_template_t();
        wrapper->impl = libreomr::Template::loadFromJsonFile(filepath);
        *out_tmpl = wrapper;
        return LIBREOMR_SUCCESS;
    } catch (const std::exception& ex) {
        setLastError(ex.what());
        return LIBREOMR_ERROR_FILE_IO;
    }
}

libreomr_status_t libreomr_template_save_json_file(const libreomr_template_t* tmpl, const char* filepath) {
    if (!tmpl || !filepath) {
        setLastError("Null pointer passed to libreomr_template_save_json_file");
        return LIBREOMR_ERROR_INVALID_ARGUMENT;
    }
    try {
        if (!tmpl->impl.saveToJsonFile(filepath)) {
            setLastError("Failed to write template file");
            return LIBREOMR_ERROR_FILE_IO;
        }
        return LIBREOMR_SUCCESS;
    } catch (const std::exception& ex) {
        setLastError(ex.what());
        return LIBREOMR_ERROR_FILE_IO;
    }
}

libreomr_status_t libreomr_template_set_fiducials(libreomr_template_t* tmpl, libreomr_quad_t fiducials) {
    if (!tmpl) return LIBREOMR_ERROR_INVALID_ARGUMENT;
    tmpl->impl.setFiducials(libreomr::Quad2D::fromC(fiducials));
    return LIBREOMR_SUCCESS;
}

libreomr_status_t libreomr_template_set_default_fiducials(
    libreomr_template_t* tmpl, float margin, float marker_size) {
    if (!tmpl) return LIBREOMR_ERROR_INVALID_ARGUMENT;
    tmpl->impl.setDefaultFiducials(margin, marker_size);
    return LIBREOMR_SUCCESS;
}

libreomr_status_t libreomr_template_add_question(
    libreomr_template_t* tmpl, const char* id, const char* name, libreomr_question_type_t type, float weight) {
    if (!tmpl || !id) return LIBREOMR_ERROR_INVALID_ARGUMENT;
    libreomr::QuestionDefinition q;
    q.id = id;
    q.name = name ? name : id;
    q.type = type;
    q.weight = weight;
    tmpl->impl.addQuestion(q);
    return LIBREOMR_SUCCESS;
}

libreomr_status_t libreomr_template_add_bubble(
    libreomr_template_t* tmpl, const char* question_id, const char* label,
    float center_x, float center_y, float radius, libreomr_bubble_shape_t shape) {
    if (!tmpl || !question_id || !label) return LIBREOMR_ERROR_INVALID_ARGUMENT;
    auto* q = tmpl->impl.findQuestion(question_id);
    if (!q) {
        setLastError("Question not found in template");
        return LIBREOMR_ERROR_NOT_FOUND;
    }
    q->addBubble(label, libreomr::Point2Df(center_x, center_y), radius, shape);
    return LIBREOMR_SUCCESS;
}

libreomr_status_t libreomr_template_set_answer_key(
    libreomr_template_t* tmpl, const char* question_id, const char* correct_answer) {
    if (!tmpl || !question_id || !correct_answer) return LIBREOMR_ERROR_INVALID_ARGUMENT;
    tmpl->impl.setAnswerKey(question_id, correct_answer);
    return LIBREOMR_SUCCESS;
}

libreomr_status_t libreomr_template_render_blank_sheet(
    const libreomr_template_t* tmpl, libreomr_image_t** out_sheet_img) {
    if (!tmpl || !out_sheet_img) return LIBREOMR_ERROR_INVALID_ARGUMENT;
    try {
        auto* wrapper = new libreomr_image_t();
        wrapper->impl = tmpl->impl.renderBlankSheet();
        *out_sheet_img = wrapper;
        return LIBREOMR_SUCCESS;
    } catch (const std::exception& ex) {
        setLastError(ex.what());
        return LIBREOMR_ERROR_INTERNAL;
    }
}

void libreomr_template_destroy(libreomr_template_t* tmpl) {
    delete tmpl;
}

libreomr_status_t libreomr_processor_create(libreomr_processor_t** out_proc) {
    if (!out_proc) return LIBREOMR_ERROR_INVALID_ARGUMENT;
    try {
        *out_proc = new libreomr_processor_t();
        return LIBREOMR_SUCCESS;
    } catch (const std::exception& ex) {
        setLastError(ex.what());
        return LIBREOMR_ERROR_OUT_OF_MEMORY;
    }
}

libreomr_status_t libreomr_processor_set_fill_threshold(libreomr_processor_t* proc, float threshold) {
    if (!proc) return LIBREOMR_ERROR_INVALID_ARGUMENT;
    proc->impl.setFillThreshold(threshold);
    return LIBREOMR_SUCCESS;
}

libreomr_status_t libreomr_processor_set_auto_align(libreomr_processor_t* proc, bool enable) {
    if (!proc) return LIBREOMR_ERROR_INVALID_ARGUMENT;
    proc->impl.setAutoAlign(enable);
    return LIBREOMR_SUCCESS;
}

libreomr_status_t libreomr_processor_process(
    libreomr_processor_t* proc, const libreomr_image_t* image,
    const libreomr_template_t* tmpl, libreomr_page_result_t** out_result) {
    if (!proc || !image || !tmpl || !out_result) {
        setLastError("Null pointer passed to libreomr_processor_process");
        return LIBREOMR_ERROR_INVALID_ARGUMENT;
    }
    try {
        libreomr::PageResult cpp_res = proc->impl.process(image->impl, tmpl->impl);
        *out_result = cpp_res.toC();
        return LIBREOMR_SUCCESS;
    } catch (const std::exception& ex) {
        setLastError(ex.what());
        return LIBREOMR_ERROR_PROCESSING;
    }
}

void libreomr_processor_destroy(libreomr_processor_t* proc) {
    delete proc;
}

static char* duplicateString(const std::string& str) {
    char* copy = static_cast<char*>(std::malloc(str.size() + 1));
    if (copy) {
        std::memcpy(copy, str.c_str(), str.size() + 1);
    }
    return copy;
}

libreomr_status_t libreomr_result_to_json(const libreomr_page_result_t* result, char** out_json_str) {
    if (!result || !out_json_str) return LIBREOMR_ERROR_INVALID_ARGUMENT;

    // Convert from C struct to JSON directly
    libreomr::JsonValue root(libreomr::JsonType::Object);
    root.set("alignment_successful", libreomr::JsonValue(result->alignment_successful));
    root.set("alignment_skew_degrees", libreomr::JsonValue(result->alignment_skew_degrees));
    root.set("total_score", libreomr::JsonValue(result->total_score));
    root.set("max_score", libreomr::JsonValue(result->max_score));
    root.set("score_percentage", libreomr::JsonValue(result->score_percentage));

    libreomr::JsonValue q_arr(libreomr::JsonType::Array);
    for (uint32_t i = 0; i < result->num_questions; ++i) {
        const auto& q = result->questions[i];
        libreomr::JsonValue q_obj(libreomr::JsonType::Object);
        q_obj.set("question_id", libreomr::JsonValue(q.question_id));
        q_obj.set("question_name", libreomr::JsonValue(q.question_name));
        q_obj.set("selected_answer", libreomr::JsonValue(q.selected_answer));
        q_obj.set("confidence", libreomr::JsonValue(q.confidence));
        q_obj.set("is_answered", libreomr::JsonValue(q.is_answered));
        q_obj.set("is_ambiguous", libreomr::JsonValue(q.is_ambiguous));
        q_obj.set("is_correct", libreomr::JsonValue(q.is_correct));
        q_obj.set("earned_score", libreomr::JsonValue(q.earned_score));
        q_obj.set("max_score", libreomr::JsonValue(q.max_score));
        q_arr.pushBack(q_obj);
    }
    root.set("questions", q_arr);

    std::string json_str = root.serialize(2);
    *out_json_str = duplicateString(json_str);
    return *out_json_str ? LIBREOMR_SUCCESS : LIBREOMR_ERROR_OUT_OF_MEMORY;
}

libreomr_status_t libreomr_result_to_csv(const libreomr_page_result_t* result, char** out_csv_str) {
    if (!result || !out_csv_str) return LIBREOMR_ERROR_INVALID_ARGUMENT;

    std::string csv = "question_id,question_name,selected_answer,is_answered,is_ambiguous,is_correct,earned_score,max_score,confidence\n";
    for (uint32_t i = 0; i < result->num_questions; ++i) {
        const auto& q = result->questions[i];
        csv += "\"" + std::string(q.question_id) + "\",\"" + std::string(q.question_name) + "\",\"" +
               std::string(q.selected_answer) + "\"," + (q.is_answered ? "true" : "false") + "," +
               (q.is_ambiguous ? "true" : "false") + "," + (q.is_correct ? "true" : "false") + "," +
               std::to_string(q.earned_score) + "," + std::to_string(q.max_score) + "," +
               std::to_string(q.confidence) + "\n";
    }

    *out_csv_str = duplicateString(csv);
    return *out_csv_str ? LIBREOMR_SUCCESS : LIBREOMR_ERROR_OUT_OF_MEMORY;
}

libreomr_status_t libreomr_result_render_debug(
    const libreomr_page_result_t* result, const libreomr_image_t* source_image, libreomr_image_t** out_debug_img) {
    if (!result || !source_image || !out_debug_img) return LIBREOMR_ERROR_INVALID_ARGUMENT;
    if (source_image->impl.empty()) {
        setLastError("Source image is empty");
        return LIBREOMR_ERROR_INVALID_ARGUMENT;
    }
    try {
        auto* wrapper = new libreomr_image_t();
        libreomr::Image overlay = (source_image->impl.channels() == 1) ? source_image->impl.toRGB() : source_image->impl.clone();

        const std::vector<uint8_t> blue = {30, 120, 240};
        const std::vector<uint8_t> green = {0, 210, 50};
        const std::vector<uint8_t> orange = {255, 150, 0};
        const std::vector<uint8_t> light_red = {230, 70, 70};

        libreomr::Quad2D det_quad = libreomr::Quad2D::fromC(result->detected_markers);
        libreomr::Quad2D tmpl_quad = libreomr::Quad2D::fromC(result->template_markers);
        overlay.drawQuad(det_quad, blue, 3);

        bool has_transform = false;
        libreomr::Matrix3x3 tmpl_to_base;
        if (result->alignment_successful && tmpl_quad.area() > 1.0 && det_quad.area() > 1.0) {
            bool h_ok = false;
            tmpl_to_base = libreomr::Matrix3x3::computeHomography(tmpl_quad, det_quad, &h_ok);
            has_transform = h_ok;
        }

        for (uint32_t i = 0; i < result->num_questions; ++i) {
            const auto& q = result->questions[i];
            for (uint32_t j = 0; j < q.num_bubbles; ++j) {
                const auto& b = q.bubbles[j];
                libreomr::Point2Df center_pt(
                    b.bounds.x + b.bounds.width / 2.0f,
                    b.bounds.y + b.bounds.height / 2.0f);
                float r = b.bounds.width / 2.0f;

                if (has_transform) {
                    libreomr::Point2Df mapped_center = tmpl_to_base.transform(center_pt);
                    libreomr::Point2Df mapped_edge = tmpl_to_base.transform({center_pt.x + r, center_pt.y});
                    r = mapped_center.distanceTo(mapped_edge);
                    center_pt = mapped_center;
                }

                libreomr::Point2Di center(
                    static_cast<int>(std::round(center_pt.x)),
                    static_cast<int>(std::round(center_pt.y)));
                int draw_r = std::max(1, static_cast<int>(std::round(r)));

                if (b.is_marked) {
                    overlay.drawCircle(center, draw_r + 2, q.is_ambiguous ? orange : green, 3);
                } else {
                    overlay.drawCircle(center, draw_r, light_red, 1);
                }
            }
        }

        wrapper->impl = std::move(overlay);
        *out_debug_img = wrapper;
        return LIBREOMR_SUCCESS;
    } catch (const std::exception& ex) {
        setLastError(ex.what());
        return LIBREOMR_ERROR_INTERNAL;
    }
}

void libreomr_result_destroy(libreomr_page_result_t* result) {
    libreomr::PageResult::freeC(result);
}

void libreomr_string_free(char* str) {
    std::free(str);
}

}
