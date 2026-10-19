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

#include "libreomr/image.hpp"
#include <algorithm>
#include <fstream>
#include <stdexcept>

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored "-Wtype-limits"
#endif

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

namespace libreomr {

Image Image::loadFromFile(const std::string& filepath) {
    int w = 0, h = 0, channels_in_file = 0;
    stbi_uc* data = stbi_load(filepath.c_str(), &w, &h, &channels_in_file, 0);
    if (!data) {
        throw std::runtime_error("Failed to load image from '" + filepath + "': " + stbi_failure_reason());
    }

    Image img(static_cast<uint32_t>(w), static_cast<uint32_t>(h), static_cast<uint32_t>(channels_in_file), data);
    stbi_image_free(data);
    return img;
}

Image Image::loadFromMemory(const uint8_t* buffer, size_t length) {
    if (!buffer || length == 0) {
        throw std::invalid_argument("Empty memory buffer provided to loadFromMemory");
    }

    int w = 0, h = 0, channels_in_file = 0;
    stbi_uc* data = stbi_load_from_memory(buffer, static_cast<int>(length), &w, &h, &channels_in_file, 0);
    if (!data) {
        throw std::runtime_error("Failed to load image from memory: " + std::string(stbi_failure_reason()));
    }

    Image img(static_cast<uint32_t>(w), static_cast<uint32_t>(h), static_cast<uint32_t>(channels_in_file), data);
    stbi_image_free(data);
    return img;
}

bool Image::saveToFile(const std::string& filepath) const {
    if (empty()) return false;

    // Detect format by extension
    std::string ext;
    size_t dot_pos = filepath.find_last_of('.');
    if (dot_pos != std::string::npos) {
        ext = filepath.substr(dot_pos + 1);
        std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return std::tolower(c); });
    }

    int w = static_cast<int>(width_);
    int h = static_cast<int>(height_);
    int comp = static_cast<int>(channels_);

    if (ext == "png") {
        return stbi_write_png(filepath.c_str(), w, h, comp, data_.data(), w * comp) != 0;
    } else if (ext == "bmp") {
        return stbi_write_bmp(filepath.c_str(), w, h, comp, data_.data()) != 0;
    } else if (ext == "jpg" || ext == "jpeg") {
        return stbi_write_jpg(filepath.c_str(), w, h, comp, data_.data(), 90) != 0;
    } else if (ext == "tga") {
        return stbi_write_tga(filepath.c_str(), w, h, comp, data_.data()) != 0;
    }

    // Default to PNG
    return stbi_write_png(filepath.c_str(), w, h, comp, data_.data(), w * comp) != 0;
}

static void memoryWriteCallback(void* context, void* data, int size) {
    auto* vec = static_cast<std::vector<uint8_t>*>(context);
    const auto* src = static_cast<const uint8_t*>(data);
    vec->insert(vec->end(), src, src + size);
}

std::vector<uint8_t> Image::encodePng() const {
    std::vector<uint8_t> result;
    if (empty()) return result;

    int w = static_cast<int>(width_);
    int h = static_cast<int>(height_);
    int comp = static_cast<int>(channels_);

    stbi_write_png_to_func(memoryWriteCallback, &result, w, h, comp, data_.data(), w * comp);
    return result;
}

} // namespace libreomr
