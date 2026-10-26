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
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
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
              << "  generate-sheet      Render a printable/scannable answer sheet\n"
              << "  generate-template   Generate a standardized examination template JSON file\n"
              << "  version             Display version and license information\n"
              << "  help                Display this help message\n\n"
              << "Process Options:\n"
              << "  --image <path>              Path to scanned sheet image (PNG, JPG, BMP) [Required]\n"
              << "  --template <path>           Path to template JSON file (or use --questions / --options)\n"
              << "  --questions <num>           Number of questions (used if --template is omitted)\n"
              << "  --options <num>             Number of options per question (default: 5)\n"
              << "  --output <path>             Output file for results (.json or .csv)\n"
              << "  --debug-image <path>        Render debug overlay image with detected marks\n"
              << "  --threshold <float>         Fill threshold ratio (default: 0.32)\n"
              << "  --no-align                  Disable fiducial corner detection & perspective unwarping\n"
              << "  --no-orient                 Disable auto-orientation detection (0/90/180/270 deg)\n"
              << "  --no-adaptive-threshold     Use global Otsu binarization instead of adaptive thresholding\n"
              << "  --no-refine-bubbles         Disable local bubble centroid refinement\n"
              << "  --csv-summary               Output concise single-line CSV summary to stdout\n\n"
              << "Generate Sheet Options:\n"
              << "  --template <path>           Path to template JSON file\n"
              << "  --questions <num>           Number of questions (if generating on the fly, default: 20)\n"
              << "  --options <num>             Options per question (default: 5 [A-E])\n"
              << "  --title <text>              Title on sheet header (default: 'Examination Answer Sheet')\n"
              << "  --page-size <A4|Letter>     Sheet page size (default: A4)\n"
              << "  --columns <num>             Layout columns (default: 0 = automatic)\n"
              << "  --dpi <num>                 Print resolution (default: 300)\n"
              << "  --output <path>             Destination image path (PNG or BMP, default: sheet.png)\n"
              << "  --clean                     Render only bubbles and fiducials without headers/decorations\n"
              << "  --with-key <keys>           Render pre-filled answer key sheet (e.g. 'ABCD...' or 'A,B,C,D')\n\n"
              << "Generate Template Options:\n"
              << "  --title <text>              Examination title (default: 'Examination Answer Sheet')\n"
              << "  --questions <num>           Number of questions (default: 20)\n"
              << "  --options <num>             Number of options per question (default: 5 [A-E])\n"
              << "  --columns <num>             Layout columns (default: 0 = automatic)\n"
              << "  --page-size <A4|Letter>     Sheet size standard (default: A4)\n"
              << "  --dpi <num>                 Target resolution (default: 300)\n"
              << "  --bubble-shape <circle|sq>  Bubble shape: circle or square (default: circle)\n"
              << "  --answer-key <keys>         Preset answer keys (e.g. 'ABCD...' or 'A,B,C,D')\n"
              << "  --output <path>             Destination JSON path (default: template.json)\n"
              << "  --render-sheet <path>       Render and save sheet image alongside JSON\n"
              << "  --clean                     Omit headers/decorations when rendering sheet\n\n"
              << "Examples:\n"
              << "  libreomr generate-template --title \"Midterm Exam\" --questions 40 --options 5 --output exam.json --render-sheet exam.png\n"
              << "  libreomr generate-sheet --questions 50 --options 4 --title \"Quiz #3\" --output quiz.png\n"
              << "  libreomr process --image student.jpg --template exam.json --output results.csv --debug-image debug.png\n";
}

std::vector<std::string> parseAnswerKeyList(const std::string& input) {
    std::vector<std::string> keys;
    if (input.empty()) return keys;

    if (input.find(',') != std::string::npos || input.find(';') != std::string::npos) {
        size_t start = 0;
        while (start < input.size()) {
            size_t end = input.find_first_of(",;", start);
            if (end == std::string::npos) end = input.size();

            size_t s = start;
            while (s < end && std::isspace(static_cast<unsigned char>(input[s]))) s++;
            size_t e = end;
            while (e > s && std::isspace(static_cast<unsigned char>(input[e - 1]))) e--;

            if (e > s) {
                keys.push_back(input.substr(s, e - s));
            }
            start = end + 1;
        }
    } else {
        // Continuous string of characters e.g. "ABCDEABCDE"
        for (char c : input) {
            if (!std::isspace(static_cast<unsigned char>(c))) {
                keys.emplace_back(1, static_cast<char>(std::toupper(static_cast<unsigned char>(c))));
            }
        }
    }
    return keys;
}

std::map<std::string, std::string> parseAnswersMap(const std::string& input, const std::vector<libreomr::QuestionDefinition>& questions) {
    std::map<std::string, std::string> map;
    auto tokens = parseAnswerKeyList(input);

    for (size_t i = 0; i < tokens.size(); ++i) {
        const std::string& tok = tokens[i];
        size_t eq = tok.find('=');
        if (eq != std::string::npos) {
            std::string q_id = tok.substr(0, eq);
            std::string ans = tok.substr(eq + 1);
            map[q_id] = ans;
        } else if (i < questions.size()) {
            map[questions[i].id] = tok;
        }
    }
    return map;
}

int handleProcess(int argc, char** argv) {
    std::string image_path;
    std::string template_path;
    std::string output_path;
    std::string debug_path;
    int num_questions = 0;
    int num_options = 5;
    float threshold = 0.32f;
    bool auto_align = true;
    bool auto_orient = true;
    bool adaptive_thresholding = true;
    bool refine_bubble_centers = true;
    bool csv_summary = false;

    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--image" && i + 1 < argc) {
            image_path = argv[++i];
        } else if (arg == "--template" && i + 1 < argc) {
            template_path = argv[++i];
        } else if (arg == "--questions" && i + 1 < argc) {
            num_questions = std::stoi(argv[++i]);
        } else if (arg == "--options" && i + 1 < argc) {
            num_options = std::stoi(argv[++i]);
        } else if (arg == "--output" && i + 1 < argc) {
            output_path = argv[++i];
        } else if (arg == "--debug-image" && i + 1 < argc) {
            debug_path = argv[++i];
        } else if (arg == "--threshold" && i + 1 < argc) {
            threshold = std::stof(argv[++i]);
        } else if (arg == "--no-align") {
            auto_align = false;
        } else if (arg == "--auto-orient") {
            auto_orient = true;
        } else if (arg == "--no-auto-orient" || arg == "--no-orient") {
            auto_orient = false;
        } else if (arg == "--adaptive-threshold") {
            adaptive_thresholding = true;
        } else if (arg == "--no-adaptive-threshold") {
            adaptive_thresholding = false;
        } else if (arg == "--refine-bubbles") {
            refine_bubble_centers = true;
        } else if (arg == "--no-refine-bubbles") {
            refine_bubble_centers = false;
        } else if (arg == "--csv-summary") {
            csv_summary = true;
        }
    }

    if (image_path.empty()) {
        std::cerr << "Error: --image is required for process command.\n";
        return 1;
    }

    try {
        libreomr::Template tmpl;
        if (!template_path.empty()) {
            if (!csv_summary) {
                std::cout << "[LibreOMR] Loading template from " << template_path << "...\n";
            }
            tmpl = libreomr::Template::loadFromJsonFile(template_path);
        } else if (num_questions > 0) {
            if (!csv_summary) {
                std::cout << "[LibreOMR] Generating on-the-fly standard template ("
                          << num_questions << " questions, " << num_options << " options)...\n";
            }
            tmpl = libreomr::Template::createStandard(num_questions, num_options);
        } else {
            std::cerr << "Error: Either --template <path> or --questions <num> must be provided.\n";
            return 1;
        }

        if (!csv_summary) {
            std::cout << "[LibreOMR] Loading image from " << image_path << "...\n";
        }
        libreomr::Image img = libreomr::Image::loadFromFile(image_path);

        libreomr::ProcessorConfig config;
        config.fill_threshold = threshold;
        config.auto_align = auto_align;
        config.auto_orient = auto_orient;
        config.adaptive_thresholding = adaptive_thresholding;
        config.refine_bubble_centers = refine_bubble_centers;

        libreomr::Processor processor(config);
        if (!csv_summary) {
            std::cout << "[LibreOMR] Processing answer sheet...\n";
        }
        libreomr::PageResult result = processor.process(img, tmpl);

        if (csv_summary) {
            std::cout << image_path << ","
                      << (result.alignment_successful ? "aligned" : "fallback") << ","
                      << result.alignment_skew_degrees << ","
                      << result.questions.size() << ","
                      << result.total_score << ","
                      << result.max_score << ","
                      << result.score_percentage << "%";
            for (const auto& q : result.questions) {
                std::cout << "," << (q.is_answered ? q.selected_answer : "");
            }
            std::cout << "\n";
        } else {
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
        }

        if (!output_path.empty()) {
            if (output_path.size() >= 4 && output_path.substr(output_path.size() - 4) == ".csv") {
                result.saveToCsvFile(output_path);
            } else {
                result.saveToJsonFile(output_path);
            }
            if (!csv_summary) {
                std::cout << "[LibreOMR] Results written to " << output_path << "\n";
            }
        }

        if (!debug_path.empty()) {
            libreomr::Image debug_img = result.renderDebugImage(img);
            debug_img.saveToFile(debug_path);
            if (!csv_summary) {
                std::cout << "[LibreOMR] Debug overlay image saved to " << debug_path << "\n";
            }
        }

        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error during processing: " << ex.what() << "\n";
        return 1;
    }
}

int handleGenerateSheet(int argc, char** argv) {
    std::string template_path;
    std::string output_path = "sheet.png";
    std::string title = "Examination Answer Sheet";
    std::string page_size = "A4";
    std::string with_key;
    int num_questions = 20;
    int num_options = 5;
    int columns = 0;
    uint32_t dpi = 300;
    bool clean = false;

    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--template" && i + 1 < argc) {
            template_path = argv[++i];
        } else if (arg == "--output" && i + 1 < argc) {
            output_path = argv[++i];
        } else if (arg == "--title" && i + 1 < argc) {
            title = argv[++i];
        } else if (arg == "--page-size" && i + 1 < argc) {
            page_size = argv[++i];
        } else if (arg == "--questions" && i + 1 < argc) {
            num_questions = std::stoi(argv[++i]);
        } else if (arg == "--options" && i + 1 < argc) {
            num_options = std::stoi(argv[++i]);
        } else if (arg == "--columns" && i + 1 < argc) {
            columns = std::stoi(argv[++i]);
        } else if (arg == "--dpi" && i + 1 < argc) {
            dpi = static_cast<uint32_t>(std::stoul(argv[++i]));
        } else if (arg == "--clean") {
            clean = true;
        } else if (arg == "--with-key" && i + 1 < argc) {
            with_key = argv[++i];
        }
    }

    try {
        libreomr::Template tmpl;
        if (!template_path.empty()) {
            std::cout << "[LibreOMR] Loading template from " << template_path << "...\n";
            tmpl = libreomr::Template::loadFromJsonFile(template_path);
        } else {
            std::cout << "[LibreOMR] Generating standard sheet ("
                      << num_questions << " questions, " << num_options << " options, "
                      << page_size << " @ " << dpi << " DPI)...\n";
            tmpl = libreomr::Template::createStandard(num_questions, num_options, title, dpi, page_size, columns);
        }

        libreomr::Image sheet;
        if (!with_key.empty()) {
            auto ans_map = parseAnswersMap(with_key, tmpl.questions());
            sheet = tmpl.renderFilledSheet(ans_map, !clean);
            std::cout << "[LibreOMR] Rendered sheet pre-filled with " << ans_map.size() << " marked answers.\n";
        } else {
            sheet = tmpl.renderBlankSheet(!clean);
        }

        sheet.saveToFile(output_path);
        std::cout << "[LibreOMR] Answer sheet successfully generated and saved to " << output_path
                  << " (" << sheet.width() << "x" << sheet.height() << ")\n";
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error generating sheet: " << ex.what() << "\n";
        return 1;
    }
}

int handleGenerateTemplate(int argc, char** argv) {
    int num_questions = 20;
    int num_options = 5;
    int columns = 0;
    uint32_t dpi = 300;
    std::string title = "Examination Answer Sheet";
    std::string page_size = "A4";
    std::string bubble_shape = "circle";
    std::string answer_keys;
    std::string output_path = "template.json";
    std::string render_sheet_path;
    bool clean = false;

    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--questions" && i + 1 < argc) {
            num_questions = std::stoi(argv[++i]);
        } else if (arg == "--options" && i + 1 < argc) {
            num_options = std::stoi(argv[++i]);
        } else if (arg == "--columns" && i + 1 < argc) {
            columns = std::stoi(argv[++i]);
        } else if (arg == "--dpi" && i + 1 < argc) {
            dpi = static_cast<uint32_t>(std::stoul(argv[++i]));
        } else if (arg == "--title" && i + 1 < argc) {
            title = argv[++i];
        } else if (arg == "--page-size" && i + 1 < argc) {
            page_size = argv[++i];
        } else if (arg == "--bubble-shape" && i + 1 < argc) {
            bubble_shape = argv[++i];
        } else if (arg == "--answer-key" && i + 1 < argc) {
            answer_keys = argv[++i];
        } else if (arg == "--output" && i + 1 < argc) {
            output_path = argv[++i];
        } else if (arg == "--render-sheet" && i + 1 < argc) {
            render_sheet_path = argv[++i];
        } else if (arg == "--clean") {
            clean = true;
        }
    }

    try {
        libreomr::Template tmpl = libreomr::Template::createStandard(
            num_questions, num_options, title, dpi, page_size, columns);

        // Customize bubble shape if requested
        if (bubble_shape == "square" || bubble_shape == "rect" || bubble_shape == "rectangle") {
            for (auto& q : tmpl.questions()) {
                for (auto& b : q.bubbles) {
                    b.shape = LIBREOMR_SHAPE_RECTANGLE;
                }
            }
        }

        // Apply answer key if specified
        if (!answer_keys.empty()) {
            auto keys = parseAnswerKeyList(answer_keys);
            for (size_t i = 0; i < keys.size() && i < tmpl.questions().size(); ++i) {
                tmpl.questions()[i].correct_answer = keys[i];
            }
        }

        if (!tmpl.saveToJsonFile(output_path)) {
            std::cerr << "Error: Failed to write template to " << output_path << "\n";
            return 1;
        }
        std::cout << "[LibreOMR] Standard template saved to " << output_path << " ("
                  << tmpl.questions().size() << " questions, " << tmpl.width() << "x" << tmpl.height() << ")\n";

        if (!render_sheet_path.empty()) {
            libreomr::Image sheet = tmpl.renderBlankSheet(!clean);
            sheet.saveToFile(render_sheet_path);
            std::cout << "[LibreOMR] Blank answer sheet rendered and saved to " << render_sheet_path << "\n";
        }

        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error creating template: " << ex.what() << "\n";
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
