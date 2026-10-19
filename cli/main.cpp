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

#include "libreomr/libreomr.hpp"
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace {

void printVersion() {
    std::cout << "LibreOMR CLI v" << libreomr_version_string() << "\n"
              << "Copyright (C) 2026 Bernardo Alvim\n"
              << "License: GNU General Public License v3.0 (GPLv3)\n"
              << "Modern Optical Mark Recognition Library\n";
}

void printHelp() {
    printVersion();
    std::cout << "\nUsage: libreomr <command> [options]\n\n"
              << "Commands:\n"
              << "  process             Analyze a scanned answer sheet against a template\n"
              << "  generate-sheet      Render a printable/scannable blank answer sheet from template\n"
              << "  generate-template   Generate a new standard template JSON file\n"
              << "  version             Display version and license information\n"
              << "  help                Display this help message\n\n"
              << "Process Options:\n"
              << "  --image <path>          Path to scanned sheet image (PNG, JPG, BMP)\n"
              << "  --template <path>       Path to template JSON file\n"
              << "  --output <path>         Output file for results (.json or .csv)\n"
              << "  --debug-image <path>    Render debug overlay image with detected marks\n"
              << "  --threshold <float>     Fill threshold ratio (default: 0.32)\n"
              << "  --no-align              Disable fiducial corner detection & perspective unwarping\n\n"
              << "Generate Sheet Options:\n"
              << "  --template <path>       Path to template JSON file\n"
              << "  --output <path>         Destination image path (.png or .bmp)\n\n"
              << "Generate Template Options:\n"
              << "  --questions <num>       Number of questions (default: 20)\n"
              << "  --options <num>         Number of options per question (default: 5 [A-E])\n"
              << "  --output <path>         Destination JSON path (default: template.json)\n";
}

int handleProcess(int argc, char** argv) {
    std::string image_path;
    std::string template_path;
    std::string output_path;
    std::string debug_path;
    float threshold = 0.32f;
    bool auto_align = true;

    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--image" && i + 1 < argc) {
            image_path = argv[++i];
        } else if (arg == "--template" && i + 1 < argc) {
            template_path = argv[++i];
        } else if (arg == "--output" && i + 1 < argc) {
            output_path = argv[++i];
        } else if (arg == "--debug-image" && i + 1 < argc) {
            debug_path = argv[++i];
        } else if (arg == "--threshold" && i + 1 < argc) {
            threshold = std::stof(argv[++i]);
        } else if (arg == "--no-align") {
            auto_align = false;
        }
    }

    if (image_path.empty() || template_path.empty()) {
        std::cerr << "Error: --image and --template are required for process command.\n";
        return 1;
    }

    try {
        std::cout << "[LibreOMR] Loading template from " << template_path << "...\n";
        libreomr::Template tmpl = libreomr::Template::loadFromJsonFile(template_path);

        std::cout << "[LibreOMR] Loading image from " << image_path << "...\n";
        libreomr::Image img = libreomr::Image::loadFromFile(image_path);

        libreomr::ProcessorConfig config;
        config.fill_threshold = threshold;
        config.auto_align = auto_align;

        libreomr::Processor processor(config);
        std::cout << "[LibreOMR] Processing answer sheet...\n";
        libreomr::PageResult result = processor.process(img, tmpl);

        std::cout << "\n================= OMR RESULTS =================\n";
        std::cout << "Alignment Successful : " << (result.alignment_successful ? "Yes" : "No (fallback used)") << "\n";
        std::cout << "Detected Skew Angle  : " << result.alignment_skew_degrees << " degrees\n";
        std::cout << "Total Questions      : " << result.questions.size() << "\n";
        if (result.max_score > 0.0f) {
            std::cout << "Score                : " << result.total_score << " / " << result.max_score
                      << " (" << result.score_percentage << "%)\n";
        }
        std::cout << "-----------------------------------------------\n";
        for (const auto& q : result.questions) {
            std::cout << q.question_id << " (" << q.question_name << "): "
                      << (q.is_answered ? q.selected_answer : "[BLANK]");
            if (q.is_ambiguous) std::cout << " [AMBIGUOUS]";
            if (q.max_score > 0.0f && !q.selected_answer.empty()) {
                std::cout << " -> " << (q.is_correct ? "CORRECT" : "WRONG");
            }
            std::cout << " (conf: " << static_cast<int>(q.confidence * 100) << "%)\n";
        }
        std::cout << "===============================================\n\n";

        if (!output_path.empty()) {
            if (output_path.size() >= 4 && output_path.substr(output_path.size() - 4) == ".csv") {
                result.saveToCsvFile(output_path);
            } else {
                result.saveToJsonFile(output_path);
            }
            std::cout << "[LibreOMR] Results written to " << output_path << "\n";
        }

        if (!debug_path.empty()) {
            libreomr::Image debug_img = result.renderDebugImage(img);
            debug_img.saveToFile(debug_path);
            std::cout << "[LibreOMR] Debug overlay image saved to " << debug_path << "\n";
        }

        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error during processing: " << ex.what() << "\n";
        return 1;
    }
}

int handleGenerateSheet(int argc, char** argv) {
    std::string template_path;
    std::string output_path = "blank_sheet.png";

    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--template" && i + 1 < argc) {
            template_path = argv[++i];
        } else if (arg == "--output" && i + 1 < argc) {
            output_path = argv[++i];
        }
    }

    if (template_path.empty()) {
        std::cerr << "Error: --template is required for generate-sheet command.\n";
        return 1;
    }

    try {
        libreomr::Template tmpl = libreomr::Template::loadFromJsonFile(template_path);
        libreomr::Image sheet = tmpl.renderBlankSheet();
        sheet.saveToFile(output_path);
        std::cout << "[LibreOMR] Generated sheet image successfully saved to " << output_path << "\n";
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }
}

int handleGenerateTemplate(int argc, char** argv) {
    int num_questions = 20;
    int num_options = 5;
    std::string output_path = "template.json";

    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--questions" && i + 1 < argc) {
            num_questions = std::stoi(argv[++i]);
        } else if (arg == "--options" && i + 1 < argc) {
            num_options = std::stoi(argv[++i]);
        } else if (arg == "--output" && i + 1 < argc) {
            output_path = argv[++i];
        }
    }

    uint32_t sheet_w = 1650;
    uint32_t sheet_h = 2330;
    libreomr::Template tmpl("Standard 20-Question Sheet", sheet_w, sheet_h, 300);
    tmpl.setDefaultFiducials(60.0f, 36.0f);

    float start_y = 220.0f;
    float row_height = (sheet_h - 400.0f) / std::max(1, (num_questions + 1) / 2);
    float col1_x = 220.0f;
    float col2_x = sheet_w / 2.0f + 120.0f;
    float bubble_spacing = 45.0f;
    float bubble_radius = 14.0f;

    for (int q = 1; q <= num_questions; ++q) {
        libreomr::QuestionDefinition q_def;
        q_def.id = "q" + std::to_string(q);
        q_def.name = "Question " + std::to_string(q);
        q_def.type = LIBREOMR_QUESTION_SINGLE_CHOICE;
        q_def.weight = 1.0f;

        bool is_col2 = (q > (num_questions / 2));
        int row_in_col = is_col2 ? (q - (num_questions / 2) - 1) : (q - 1);
        float base_x = is_col2 ? col2_x : col1_x;
        float base_y = start_y + row_in_col * row_height;

        for (int opt = 0; opt < num_options; ++opt) {
            std::string label(1, static_cast<char>('A' + opt));
            float bx = base_x + opt * bubble_spacing;
            q_def.addBubble(label, libreomr::Point2Df(bx, base_y), bubble_radius);
        }

        tmpl.addQuestion(q_def);
    }

    if (tmpl.saveToJsonFile(output_path)) {
        std::cout << "[LibreOMR] Template successfully saved to " << output_path << "\n";
        return 0;
    } else {
        std::cerr << "Failed to save template to " << output_path << "\n";
        return 1;
    }
}

} // namespace

int main(int argc, char** argv) {
    if (argc < 2) {
        printHelp();
        return 0;
    }

    std::string cmd = argv[1];
    if (cmd == "process") {
        return handleProcess(argc, argv);
    } else if (cmd == "generate-sheet") {
        return handleGenerateSheet(argc, argv);
    } else if (cmd == "generate-template") {
        return handleGenerateTemplate(argc, argv);
    } else if (cmd == "version" || cmd == "--version" || cmd == "-v") {
        printVersion();
        return 0;
    } else if (cmd == "help" || cmd == "--help" || cmd == "-h") {
        printHelp();
        return 0;
    } else {
        std::cerr << "Unknown command: " << cmd << "\n\n";
        printHelp();
        return 1;
    }
}
