# LibreOMR

[![CI](https://github.com/BernardoAlvim/LibreOMR/actions/workflows/ci.yml/badge.svg)](https://github.com/BernardoAlvim/LibreOMR/actions/workflows/ci.yml)
[![Release](https://github.com/BernardoAlvim/LibreOMR/actions/workflows/release.yml/badge.svg)](https://github.com/BernardoAlvim/LibreOMR/actions/workflows/release.yml)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Standard](https://img.shields.io/badge/C%2B%2B-17-purple.svg)](https://en.cppreference.com/w/cpp/17)
[![SemVer](https://img.shields.io/badge/SemVer-1.0.0-green.svg)](https://semver.org/)

**LibreOMR** is a modern, high-performance, open-source Optical Mark Recognition (OMR) library implemented in C++17 with a pure C ABI (`libreomr.h`). It provides an end-to-end processing pipeline for grading bubble answer sheets, multiple-choice exams, and paper survey forms with automatic corner fiducial detection, perspective unwarping, adaptive local background normalization, and ambiguity classification.

---

## Key Features

- **Autonomous Document Rectification**:
  - Detects corner registration fiducial markers (solid squares / bullseyes) using fast two-pass 8-connectivity Connected Component Labeling.
  - Automatically sorts corners and computes an $8 \times 8$ Hartley-normalized Direct Linear Transformation (DLT) Projective Homography matrix.
  - Rectifies skewed and perspective-distorted scans to template canonical coordinates with bilinear sampling.
- **Photometric & Adaptive Mark Analysis**:
  - Global Otsu optimal binarization with maximal variance plateau resolution.
  - Adaptive thresholding with $O(1)$ 2D Integral Images.
  - Local paper background compensation to withstand uneven ambient lighting, shadows, and scanner contrast shifts.
  - Differential inner sampling (excluding the printed bubble outline) to avoid false positives.
  - Robust ambiguity detection (erasure marks, multi-mark conflicts on single-choice questions).
- **Dual Interface**:
  - **Modern C++ API** (`libreomr/libreomr.hpp`): Clean OOP abstractions, RAII, STL containers, exceptions, and `JsonValue` serialization.
  - **Pure C ABI** (`libreomr/libreomr.h`): Opaque pointers, status codes (`libreomr_status_t`), thread-local error messages, FFI-ready for Python (`ctypes`/`cffi`), Rust, Go (`cgo`), and C#.
- **Zero Heavy Dependencies**:
  - Self-contained mathematical engine (vectors, matrices, homography, solvers).
  - Integrated image codecs for PNG, BMP, JPEG, and TGA.
  - Built-in JSON parser and serializer with no external JSON library requirements.
- **Form Generation Engine**:
  - Synthesize printable blank answer sheets directly from JSON templates.
  - Synthesize answer key sheets for calibration and automated testing.
- **Full CLI Suite** (`libreomr`):
  - Command-line tool for template generation, sheet rendering, batch processing, and debug overlay visualization.

---

## Project Structure

```
LibreOMR/
├── .github/
│   └── workflows/
│       ├── ci.yml               # Multi-platform CI matrix (Linux, Windows, macOS)
│       └── release.yml          # Automated release packaging on v*.*.* tags
├── cmake/
│   ├── LibreOMRConfig.cmake.in  # CMake package configuration
│   └── libreomr.pc.in           # pkg-config template
├── include/
│   └── libreomr/
│       ├── binarization.hpp     # Otsu, adaptive threshold, morphology
│       ├── connected_components.hpp # Fast 8-connected two-pass blob extraction
│       ├── export.h             # Export/Import symbol macros
│       ├── fiducial.hpp         # Corner registration marker detector
│       ├── geometry.hpp         # Vector2D, Rect, Quad2D, Matrix3x3, Homography
│       ├── image.hpp            # Image buffer, drawing, filters, warps
│       ├── json.hpp             # Lightweight zero-dependency JSON parser
│       ├── libreomr.h           # Pure C ABI header
│       ├── libreomr.hpp         # Modern C++ umbrella header
│       ├── processor.hpp        # OMR engine and bubble analysis
│       ├── result.hpp           # Evaluation results, JSON/CSV exports, overlays
│       ├── template.hpp         # Layout definition and sheet generator
│       ├── types.h              # Shared types, structs, enums
│       └── version.h            # SemVer version definitions
├── src/
│   ├── binarization.cpp
│   ├── c_api.cpp                # C wrapper implementation
│   ├── connected_components.cpp
│   ├── fiducial.cpp
│   ├── geometry.cpp
│   ├── image.cpp
│   ├── image_io.cpp
│   ├── json.cpp
│   ├── processor.cpp
│   ├── result.cpp
│   ├── template.cpp
│   └── version.cpp
├── cli/
│   └── main.cpp                 # libreomr command-line interface
├── tests/
│   ├── test_c_api.cpp
│   ├── test_fiducial.cpp
│   ├── test_framework.hpp       # Lightweight test runner
│   ├── test_geometry.cpp
│   ├── test_image.cpp
│   ├── test_pipeline.cpp        # Full end-to-end distorted exam pipeline
│   ├── test_template.cpp
│   └── main.cpp
├── CMakeLists.txt
├── LICENSE                      # GNU General Public License v3.0
└── README.md
```

---

## Building from Source

### Prerequisites
- CMake 3.16 or higher
- C++17 compatible compiler (GCC 9+, Clang 10+, or MSVC 2019+)
- Ninja (recommended) or Make

### Compiling with Ninja (6 threads)
```bash
# Configure
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DLIBREOMR_BUILD_TESTS=ON -DLIBREOMR_BUILD_CLI=ON

# Build using 6 threads
ninja -C build -j6

# Run unit tests
./build/libreomr_tests
```

### Installation
```bash
cmake --install build --prefix /usr/local
```

---

## C++ API Usage Example

```cpp
#include <libreomr/libreomr.hpp>
#include <iostream>

int main() {
    // 1. Load the template definition
    libreomr::Template tmpl = libreomr::Template::loadFromJsonFile("template.json");

    // 2. Load the scanned student response sheet
    libreomr::Image sheet = libreomr::Image::loadFromFile("scanned_sheet.jpg");

    // 3. Configure the processor
    libreomr::ProcessorConfig config;
    config.fill_threshold = 0.32f;  // Mark fill threshold
    config.auto_align = true;        // Enable corner fiducial rectification

    libreomr::Processor processor(config);

    // 4. Run recognition
    libreomr::PageResult result = processor.process(sheet, tmpl);

    std::cout << "Alignment: " << (result.alignment_successful ? "Success" : "Failed") << "\n";
    std::cout << "Skew: " << result.alignment_skew_degrees << " degrees\n";
    std::cout << "Score: " << result.total_score << " / " << result.max_score
              << " (" << result.score_percentage << "%)\n";

    for (const auto& q : result.questions) {
        std::cout << q.question_id << ": " << q.selected_answer
                  << " (Confidence: " << q.confidence * 100.0f << "%)\n";
    }

    // 5. Export results
    result.saveToJsonFile("result.json");
    result.saveToCsvFile("result.csv");

    // 6. Generate visual verification overlay
    libreomr::Image overlay = result.renderDebugImage(sheet);
    overlay.saveToFile("debug_overlay.png");

    return 0;
}
```

---

## Pure C ABI Usage Example

```c
#include <libreomr/libreomr.h>
#include <stdio.h>

int main(void) {
    // 1. Load template and image
    libreomr_template_t* tmpl = NULL;
    libreomr_status_t status = libreomr_template_load_json_file("template.json", &tmpl);
    if (status != LIBREOMR_SUCCESS) {
        fprintf(stderr, "Error loading template: %s\n", libreomr_get_last_error());
        return 1;
    }

    libreomr_image_t* image = NULL;
    status = libreomr_image_load_file("sheet.png", &image);
    if (status != LIBREOMR_SUCCESS) {
        fprintf(stderr, "Error loading image: %s\n", libreomr_get_last_error());
        libreomr_template_destroy(tmpl);
        return 1;
    }

    // 2. Setup processor
    libreomr_processor_t* proc = NULL;
    libreomr_processor_create(&proc);
    libreomr_processor_set_fill_threshold(proc, 0.32f);
    libreomr_processor_set_auto_align(proc, true);

    // 3. Process
    libreomr_page_result_t* result = NULL;
    status = libreomr_processor_process(proc, image, tmpl, &result);
    if (status == LIBREOMR_SUCCESS) {
        printf("Score: %.1f / %.1f (%.1f%%)\n",
               result->total_score, result->max_score, result->score_percentage);

        for (uint32_t i = 0; i < result->num_questions; ++i) {
            libreomr_question_result_t* q = &result->questions[i];
            printf("%s: %s [conf: %.0f%%]\n",
                   q->question_id, q->selected_answer, q->confidence * 100.0f);
        }

        // Export JSON
        char* json_output = NULL;
        libreomr_result_to_json(result, &json_output);
        if (json_output) {
            printf("\nJSON Output:\n%s\n", json_output);
            libreomr_string_free(json_output);
        }

        libreomr_result_destroy(result);
    }

    // 4. Clean up
    libreomr_processor_destroy(proc);
    libreomr_image_destroy(image);
    libreomr_template_destroy(tmpl);

    return 0;
}
```

---

## CLI Tool Usage

The `libreomr` executable provides command-line utilities for all common workflows:

### 1. Generate a Standard Form Template
```bash
libreomr generate-template --questions 20 --options 5 --output exam_template.json
```

### 2. Render a Printable/Scannable Blank Answer Sheet
```bash
libreomr generate-sheet --template exam_template.json --output blank_sheet.png
```

### 3. Process a Scanned Sheet
```bash
libreomr process \
  --image scan_sample.jpg \
  --template exam_template.json \
  --output result.json \
  --debug-image debug_overlay.png \
  --threshold 0.32
```

---

## CMake Integration

Add LibreOMR to your project using CMake:

```cmake
find_package(LibreOMR REQUIRED)

add_executable(my_grader main.cpp)
target_link_libraries(my_grader PRIVATE LibreOMR::libreomr)
```

Alternatively, integrate via `add_subdirectory`:
```cmake
add_subdirectory(path/to/LibreOMR)
target_link_libraries(my_grader PRIVATE libreomr)
```

---

## License

LibreOMR is licensed under the **GNU General Public License v3.0 (GPLv3)**.  
See the [LICENSE](LICENSE) file for the full license text.

Copyright (C) 2026 Bernardo Alvim.
