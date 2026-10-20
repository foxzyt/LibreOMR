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

#ifndef LIBREOMR_PROCESSOR_HPP
#define LIBREOMR_PROCESSOR_HPP

#include "libreomr/export.h"
#include "libreomr/fiducial.hpp"
#include "libreomr/image.hpp"
#include "libreomr/result.hpp"
#include "libreomr/template.hpp"

namespace libreomr {

struct LIBREOMR_API ProcessorConfig {
    float fill_threshold{0.32f};
    float ambiguity_margin{0.12f};
    bool auto_align{true};
    bool auto_orient{true};
    bool adaptive_thresholding{true};
    bool refine_bubble_centers{true};
    float inner_sample_ratio{0.78f};
    float background_inner_ratio{1.15f};
    float background_outer_ratio{1.60f};
    FiducialConfig fiducial_config{};
};

class LIBREOMR_API Processor {
public:
    Processor();
    explicit Processor(ProcessorConfig config);

    [[nodiscard]] const ProcessorConfig& config() const noexcept { return config_; }
    void setConfig(const ProcessorConfig& config) { config_ = config; }

    [[nodiscard]] float fillThreshold() const noexcept { return config_.fill_threshold; }
    void setFillThreshold(float threshold) { config_.fill_threshold = threshold; }

    [[nodiscard]] bool autoAlign() const noexcept { return config_.auto_align; }
    void setAutoAlign(bool enable) { config_.auto_align = enable; }

    [[nodiscard]] bool autoOrient() const noexcept { return config_.auto_orient; }
    void setAutoOrient(bool enable) { config_.auto_orient = enable; }

    [[nodiscard]] bool adaptiveThresholding() const noexcept { return config_.adaptive_thresholding; }
    void setAdaptiveThresholding(bool enable) { config_.adaptive_thresholding = enable; }

    [[nodiscard]] bool refineBubbleCenters() const noexcept { return config_.refine_bubble_centers; }
    void setRefineBubbleCenters(bool enable) { config_.refine_bubble_centers = enable; }

    [[nodiscard]] PageResult process(const Image& image, const Template& tmpl) const;
    [[nodiscard]] PageResult process(const std::string& image_path, const Template& tmpl) const;
    [[nodiscard]] BubbleMetric analyzeBubble(const Image& gray_image, const Image& binary_image,
                                             const BubbleDefinition& bubble) const;

private:
    ProcessorConfig config_;
};

} // namespace libreomr

#endif /* LIBREOMR_PROCESSOR_HPP */
