/*
 * LibreOMR Desktop - Optical Mark Recognition
 * Copyright (C) 2026 Bernardo Alvim
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#define WEBVIEW_STATIC
#include "webview.h"
#include "libreomr/libreomr.hpp"
#include "libreomr/json.hpp"

#include <windows.h>
#include <commdlg.h>
#include <shellapi.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <filesystem>

namespace fs = std::filesystem;

namespace {

// Native Windows File Dialog Helpers
std::string openSingleFileDialog(const wchar_t* filter, const wchar_t* title, HWND hwndOwner = NULL) {
    wchar_t filename[MAX_PATH] = {0};
    OPENFILENAMEW ofn = {0};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwndOwner;
    ofn.lpstrFilter = filter;
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = title;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

    if (GetOpenFileNameW(&ofn)) {
        int size = WideCharToMultiByte(CP_UTF8, 0, filename, -1, NULL, 0, NULL, NULL);
        std::string utf8_path(size - 1, 0);
        WideCharToMultiByte(CP_UTF8, 0, filename, -1, &utf8_path[0], size, NULL, NULL);
        return utf8_path;
    }
    return "";
}

std::vector<std::string> openMultipleFilesDialog(const wchar_t* filter, const wchar_t* title, HWND hwndOwner = NULL) {
    std::vector<std::string> result;
    const int buffer_size = 65536;
    std::vector<wchar_t> buffer(buffer_size, 0);

    OPENFILENAMEW ofn = {0};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwndOwner;
    ofn.lpstrFilter = filter;
    ofn.lpstrFile = buffer.data();
    ofn.nMaxFile = buffer_size;
    ofn.lpstrTitle = title;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER;

    if (GetOpenFileNameW(&ofn)) {
        wchar_t* ptr = buffer.data();
        std::wstring directory = ptr;
        ptr += directory.length() + 1;

        if (*ptr == L'\0') {
            // Single file selected
            int size = WideCharToMultiByte(CP_UTF8, 0, directory.c_str(), -1, NULL, 0, NULL, NULL);
            std::string u8(size - 1, 0);
            WideCharToMultiByte(CP_UTF8, 0, directory.c_str(), -1, &u8[0], size, NULL, NULL);
            result.push_back(u8);
        } else {
            // Multiple files selected
            while (*ptr != L'\0') {
                std::wstring file = ptr;
                std::wstring full_path = directory + L"\\" + file;
                int size = WideCharToMultiByte(CP_UTF8, 0, full_path.c_str(), -1, NULL, 0, NULL, NULL);
                std::string u8(size - 1, 0);
                WideCharToMultiByte(CP_UTF8, 0, full_path.c_str(), -1, &u8[0], size, NULL, NULL);
                result.push_back(u8);
                ptr += file.length() + 1;
            }
        }
    }
    return result;
}

std::string saveFileDialog(const wchar_t* filter, const wchar_t* default_ext, const wchar_t* title, const std::string& default_name, HWND hwndOwner = NULL) {
    wchar_t filename[MAX_PATH] = {0};
    if (!default_name.empty()) {
        MultiByteToWideChar(CP_UTF8, 0, default_name.c_str(), -1, filename, MAX_PATH);
    }

    OPENFILENAMEW ofn = {0};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwndOwner;
    ofn.lpstrFilter = filter;
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrDefExt = default_ext;
    ofn.lpstrTitle = title;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

    if (GetSaveFileNameW(&ofn)) {
        int size = WideCharToMultiByte(CP_UTF8, 0, filename, -1, NULL, 0, NULL, NULL);
        std::string utf8_path(size - 1, 0);
        WideCharToMultiByte(CP_UTF8, 0, filename, -1, &utf8_path[0], size, NULL, NULL);
        return utf8_path;
    }
    return "";
}

std::string toJsonString(const std::string& s) {
    std::string json = "\"";
    for (char c : s) {
        if (c == '\\') json += "\\\\";
        else if (c == '\"') json += "\\\"";
        else if (c == '\n') json += "\\n";
        else if (c == '\r') json += "\\r";
        else if (c == '\t') json += "\\t";
        else json += c;
    }
    json += "\"";
    return json;
}

static const std::string b64_table = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

std::vector<unsigned char> base64_decode(const std::string& in) {
    std::vector<unsigned char> out;
    std::vector<int> T(256, -1);
    for (int i = 0; i < 64; i++) T[b64_table[i]] = i;

    int val = 0, valb = -8;
    for (unsigned char c : in) {
        if (T[c] == -1) continue;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            out.push_back(static_cast<unsigned char>((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return out;
}

// Built-in Template Generator
libreomr::Template createStandardTemplate(int num_questions, int num_options = 5) {
    return libreomr::Template::createStandard(
        num_questions, num_options, "Standard Examination Sheet", 300, "A4");
}

// Locate UI files
std::string findUiPath() {
    std::vector<std::string> candidates = {
        "app/ui/index.html",
        "ui/index.html",
        "../app/ui/index.html",
        "../../app/ui/index.html"
    };

    // Check module dir
    wchar_t exe_path_w[MAX_PATH] = {0};
    GetModuleFileNameW(NULL, exe_path_w, MAX_PATH);
    fs::path exe_dir = fs::path(exe_path_w).parent_path();

    std::vector<fs::path> search_paths = {
        fs::current_path() / "app" / "ui" / "index.html",
        fs::current_path() / "ui" / "index.html",
        exe_dir / "app" / "ui" / "index.html",
        exe_dir / "ui" / "index.html",
        exe_dir / ".." / "app" / "ui" / "index.html"
    };

    for (const auto& p : search_paths) {
        if (fs::exists(p)) {
            return fs::canonical(p).string();
        }
    }

    return (fs::current_path() / "app" / "ui" / "index.html").string();
}

} // namespace

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    (void)hInstance; (void)hPrevInstance; (void)lpCmdLine; (void)nCmdShow;

    try {
        webview::webview w(true, nullptr);
        w.set_title("LibreOMR Desktop");
        w.set_size(1200, 820, WEBVIEW_HINT_NONE);

        // Set Window and Taskbar Icon
        HWND hwnd = nullptr;
        try {
            hwnd = static_cast<HWND>(w.window().value());
        } catch (...) {}
        if (hwnd) {
            HICON hIconBig = (HICON)LoadImageA(hInstance, MAKEINTRESOURCEA(1), IMAGE_ICON, 32, 32, LR_SHARED);
            HICON hIconSmall = (HICON)LoadImageA(hInstance, MAKEINTRESOURCEA(1), IMAGE_ICON, 16, 16, LR_SHARED);
            if (!hIconBig) {
                hIconBig = (HICON)LoadImageW(NULL, L"app/resources/app.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
                hIconSmall = (HICON)LoadImageW(NULL, L"app/resources/app.ico", IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
            }
            if (hIconBig) SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIconBig);
            if (hIconSmall) SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconSmall);
        }

        // Parse command line arguments passed from Windows Explorer Context Menu
        std::string initialTarget = "";
        int numArgs = 0;
        LPWSTR* szArgList = CommandLineToArgvW(GetCommandLineW(), &numArgs);
        if (szArgList && numArgs > 1) {
            int size = WideCharToMultiByte(CP_UTF8, 0, szArgList[1], -1, NULL, 0, NULL, NULL);
            if (size > 1) {
                std::string utf8_path(size - 1, 0);
                WideCharToMultiByte(CP_UTF8, 0, szArgList[1], -1, &utf8_path[0], size, NULL, NULL);
                initialTarget = utf8_path;
            }
        }
        if (szArgList) LocalFree(szArgList);

        // API 0: Query initial target passed via Context Menu or CLI
        w.bind("api_get_initial_target", [initialTarget](const std::string& /*args*/) -> std::string {
            if (initialTarget.empty()) {
                return "{\"type\":\"none\"}";
            }
            try {
                fs::path p = fs::u8path(initialTarget);
                if (fs::is_directory(p)) {
                    std::vector<std::string> img_files;
                    for (const auto& entry : fs::directory_iterator(p)) {
                        if (entry.is_regular_file()) {
                            auto ext = entry.path().extension().string();
                            for (auto& c : ext) c = tolower(c);
                            if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp" || ext == ".webp") {
                                img_files.push_back(entry.path().u8string());
                            }
                        }
                    }
                    std::string json = "{\"type\":\"directory\",\"path\":\"";
                    for (char c : initialTarget) {
                        if (c == '\\') json += "\\\\";
                        else if (c == '\"') json += "\\\"";
                        else json += c;
                    }
                    json += "\",\"files\":[";
                    for (size_t i = 0; i < img_files.size(); ++i) {
                        json += "\"";
                        for (char c : img_files[i]) {
                            if (c == '\\') json += "\\\\";
                            else if (c == '\"') json += "\\\"";
                            else json += c;
                        }
                        json += "\"";
                        if (i + 1 < img_files.size()) json += ",";
                    }
                    json += "]}";
                    return json;
                } else if (fs::exists(p)) {
                    std::string json = "{\"type\":\"file\",\"path\":\"";
                    for (char c : initialTarget) {
                        if (c == '\\') json += "\\\\";
                        else if (c == '\"') json += "\\\"";
                        else json += c;
                    }
                    json += "\"}";
                    return json;
                }
            } catch (...) {}
            return "{\"type\":\"none\"}";
        });

        // API 0.5: Save dropped image from drag & drop
        w.bind("api_save_dropped_image", [](const std::string& args) -> std::string {
            try {
                auto j = libreomr::JsonValue::parse(args);
                if (j.isArray() && j.size() >= 2) {
                    std::string filename = j[0].asString();
                    std::string dataUrl = j[1].asString();
                    size_t comma = dataUrl.find(',');
                    std::string b64 = (comma != std::string::npos) ? dataUrl.substr(comma + 1) : dataUrl;
                    auto bytes = base64_decode(b64);
                    
                    wchar_t temp_dir_w[MAX_PATH];
                    GetTempPathW(MAX_PATH, temp_dir_w);
                    fs::path temp_dir(temp_dir_w);
                    fs::path safe_name = fs::path(filename).filename();
                    fs::path out_path = temp_dir / ("libreomr_" + std::to_string(GetTickCount()) + "_" + safe_name.string());
                    
                    std::ofstream out(out_path, std::ios::binary);
                    if (out.is_open()) {
                        out.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
                        out.close();
                        return toJsonString(out_path.u8string());
                    }
                }
            } catch (...) {}
            return "\"\"";
        });

        // API 1: Pick single image
        w.bind("api_pick_image", [hwnd](const std::string& /*args*/) -> std::string {
            const wchar_t* filter = L"Image Files (*.png;*.jpg;*.jpeg;*.bmp)\0*.png;*.jpg;*.jpeg;*.bmp\0All Files (*.*)\0*.*\0";
            std::string chosen = openSingleFileDialog(filter, L"Select Answer Sheet Image", hwnd);
            return toJsonString(chosen);
        });

        // API 2: Pick multiple images for batch
        w.bind("api_pick_images", [hwnd](const std::string& /*args*/) -> std::string {
            const wchar_t* filter = L"Image Files (*.png;*.jpg;*.jpeg;*.bmp)\0*.png;*.jpg;*.jpeg;*.bmp\0All Files (*.*)\0*.*\0";
            auto files = openMultipleFilesDialog(filter, L"Select Answer Sheet Images for Batch", hwnd);
            
            std::string json = "[";
            for (size_t i = 0; i < files.size(); ++i) {
                // escape backslashes
                std::string esc;
                for (char c : files[i]) {
                    if (c == '\\') esc += "\\\\";
                    else esc += c;
                }
                json += "\"" + esc + "\"";
                if (i + 1 < files.size()) json += ",";
            }
            json += "]";
            return json;
        });

        // API 3: Pick custom template
        w.bind("api_pick_template", [hwnd](const std::string& /*args*/) -> std::string {
            const wchar_t* filter = L"JSON Template (*.json)\0*.json\0All Files (*.*)\0*.*\0";
            return toJsonString(openSingleFileDialog(filter, L"Select Evaluation Template", hwnd));
        });

        // API 4: Save file dialog
        w.bind("api_save_file", [hwnd](const std::string& args) -> std::string {
            // args is [default_name, ext]
            std::string def_name = "results.csv";
            std::string ext = "csv";
            try {
                auto j = libreomr::JsonValue::parse(args);
                if (j.isArray() && j.size() >= 2) {
                    def_name = j[0].asString();
                    ext = j[1].asString();
                }
            } catch (...) {}

            const wchar_t* filter = (ext == "csv") 
                ? L"CSV File (*.csv)\0*.csv\0All Files (*.*)\0*.*\0"
                : L"JSON File (*.json)\0*.json\0All Files (*.*)\0*.*\0";
            const wchar_t* def_ext = (ext == "csv") ? L"csv" : L"json";

            std::string saved = saveFileDialog(filter, def_ext, L"Save File As", def_name, hwnd);
            return toJsonString(saved);
        });

        // API 5: Save text file to disk
        w.bind("api_save_text_file", [](const std::string& args) -> std::string {
            try {
                auto j = libreomr::JsonValue::parse(args);
                if (j.isArray() && j.size() >= 2) {
                    std::string filepath = j[0].asString();
                    std::string content = j[1].asString();
                    std::ofstream out(filepath, std::ios::binary);
                    if (out.is_open()) {
                        out << content;
                        return "true";
                    }
                }
            } catch (...) {}
            return "false";
        });

        // API 6: Process Single Sheet
        w.bind("api_process", [](const std::string& args) -> std::string {
            try {
                auto j_args = libreomr::JsonValue::parse(args);
                std::string image_path;
                std::string template_type = "configured";
                std::string custom_template_path;
                int num_questions = 40;
                int num_options = 5;
                float threshold = 0.32f;
                bool auto_align = true;
                bool auto_orient = true;
                bool adaptive_thresh = true;
                bool refine_bubbles = true;

                if (j_args.isArray() && j_args.size() > 0) {
                    auto payload = j_args[0];
                    if (payload.isString()) {
                        payload = libreomr::JsonValue::parse(payload.asString());
                    }
                    if (payload.contains("image_path")) image_path = payload["image_path"].asString();
                    if (payload.contains("template_type")) template_type = payload["template_type"].asString();
                    if (payload.contains("custom_template_path")) custom_template_path = payload["custom_template_path"].asString();
                    if (payload.contains("num_questions")) num_questions = static_cast<int>(payload["num_questions"].asDouble());
                    if (payload.contains("num_options")) num_options = static_cast<int>(payload["num_options"].asDouble());

                    if (payload.contains("config")) {
                        auto cfg = payload["config"];
                        if (cfg.contains("fill_threshold")) threshold = static_cast<float>(cfg["fill_threshold"].asDouble());
                        if (cfg.contains("auto_align")) auto_align = cfg["auto_align"].asBool();
                        if (cfg.contains("auto_orient")) auto_orient = cfg["auto_orient"].asBool();
                        if (cfg.contains("adaptive_thresholding")) adaptive_thresh = cfg["adaptive_thresholding"].asBool();
                        if (cfg.contains("refine_bubble_centers")) refine_bubbles = cfg["refine_bubble_centers"].asBool();
                    }
                }

                if (image_path.empty()) {
                    return "{\"error\": \"No image path provided\"}";
                }

                // Prepare Template
                libreomr::Template tmpl;
                if (template_type == "custom" && !custom_template_path.empty() && fs::exists(custom_template_path)) {
                    tmpl = libreomr::Template::loadFromJsonFile(custom_template_path);
                } else {
                    tmpl = createStandardTemplate(num_questions, num_options);
                }

                // Load Image
                libreomr::Image img = libreomr::Image::loadFromFile(image_path);
                if (img.empty()) {
                    return "{\"error\": \"Failed to load image file\"}";
                }

                // Configure Processor
                libreomr::ProcessorConfig config;
                config.fill_threshold = threshold;
                config.auto_align = auto_align;
                config.auto_orient = auto_orient;
                config.adaptive_thresholding = adaptive_thresh;
                config.refine_bubble_centers = refine_bubbles;

                libreomr::Processor processor(config);
                libreomr::PageResult result = processor.process(img, tmpl);

                return result.toJson();
            } catch (const std::exception& e) {
                return std::string("{\"error\": \"") + e.what() + "\"}";
            }
        });

        // API 7: Generate Blank Sheet PNG
        w.bind("api_generate_sheet", [hwnd](const std::string& args) -> std::string {
            try {
                auto j_args = libreomr::JsonValue::parse(args);
                int num_questions = 40;
                int num_options = 5;
                int dpi = 300;
                std::string lang = "pt";
                if (j_args.isArray() && j_args.size() > 0) {
                    auto p = j_args[0];
                    if (p.isString()) {
                        p = libreomr::JsonValue::parse(p.asString());
                    }
                    if (p.contains("num_questions")) num_questions = static_cast<int>(p["num_questions"].asDouble());
                    if (p.contains("num_options")) num_options = static_cast<int>(p["num_options"].asDouble());
                    if (p.contains("dpi")) dpi = static_cast<int>(p["dpi"].asDouble());
                    if (p.contains("lang")) lang = p["lang"].asString();
                }

                libreomr::Template tmpl = createStandardTemplate(num_questions, num_options);
                tmpl.setDimensions(tmpl.width(), tmpl.height(), dpi);

                std::string def_file = "folha_" + std::to_string(num_questions) + "q_" + std::to_string(num_options) + "alt_" + lang + ".png";
                std::string dest = saveFileDialog(L"PNG Image (*.png)\0*.png\0", L"png", L"Save Blank Answer Sheet", def_file, hwnd);
                if (dest.empty()) {
                    dest = def_file;
                }

                libreomr::Image sheet = tmpl.renderBlankSheet(true, lang);
                sheet.saveToFile(dest);
                return toJsonString(dest);
            } catch (const std::exception& e) {
                return toJsonString(std::string("Error: ") + e.what());
            }
        });

        // Navigate to UI
        std::string ui_file = findUiPath();
        std::string file_url = "file:///" + fs::absolute(ui_file).generic_string();
        w.navigate(file_url);

        w.run();
    } catch (const std::exception& e) {
        std::string msg = std::string("Fatal error initializing LibreOMR Desktop: ") + e.what();
        MessageBoxA(NULL, msg.c_str(), "LibreOMR Desktop Error", MB_ICONERROR | MB_OK);
        return 1;
    }

    return 0;
}
