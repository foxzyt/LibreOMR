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

#include "libreomr/json.hpp"
#include <cctype>
#include <cstdint>
#include <cstring>
#include <sstream>
#include <stdexcept>

namespace libreomr {

JsonValue::JsonValue() : type_(JsonType::Null) {}
JsonValue::JsonValue(bool val) : type_(JsonType::Boolean), bool_val_(val) {}
JsonValue::JsonValue(double val) : type_(JsonType::Number), num_val_(val) {}
JsonValue::JsonValue(int val) : type_(JsonType::Number), num_val_(static_cast<double>(val)) {}
JsonValue::JsonValue(uint32_t val) : type_(JsonType::Number), num_val_(static_cast<double>(val)) {}
JsonValue::JsonValue(const char* val) : type_(JsonType::String), str_val_(val ? val : "") {}
JsonValue::JsonValue(std::string val) : type_(JsonType::String), str_val_(std::move(val)) {}
JsonValue::JsonValue(JsonType type) : type_(type) {}

bool JsonValue::asBool(bool def) const {
    return (type_ == JsonType::Boolean) ? bool_val_ : def;
}

double JsonValue::asDouble(double def) const {
    return (type_ == JsonType::Number) ? num_val_ : def;
}

int JsonValue::asInt(int def) const {
    return (type_ == JsonType::Number) ? static_cast<int>(num_val_) : def;
}

uint32_t JsonValue::asUint(uint32_t def) const {
    return (type_ == JsonType::Number) ? static_cast<uint32_t>(num_val_) : def;
}

const std::string& JsonValue::asString() const {
    static const std::string empty_str;
    return (type_ == JsonType::String) ? str_val_ : empty_str;
}

void JsonValue::pushBack(const JsonValue& val) {
    if (type_ != JsonType::Array) {
        type_ = JsonType::Array;
        arr_val_.clear();
    }
    arr_val_.push_back(val);
}

size_t JsonValue::size() const {
    if (type_ == JsonType::Array) return arr_val_.size();
    if (type_ == JsonType::Object) return obj_val_.size();
    return 0;
}

const JsonValue& JsonValue::operator[](size_t index) const {
    static const JsonValue null_val;
    if (type_ == JsonType::Array && index < arr_val_.size()) {
        return arr_val_[index];
    }
    return null_val;
}

JsonValue& JsonValue::operator[](size_t index) {
    if (type_ != JsonType::Array) {
        type_ = JsonType::Array;
        arr_val_.clear();
    }
    if (index >= arr_val_.size()) {
        arr_val_.resize(index + 1);
    }
    return arr_val_[index];
}

const std::vector<JsonValue>& JsonValue::arrayValues() const {
    static const std::vector<JsonValue> empty_arr;
    return (type_ == JsonType::Array) ? arr_val_ : empty_arr;
}

void JsonValue::set(const std::string& key, const JsonValue& val) {
    if (type_ != JsonType::Object) {
        type_ = JsonType::Object;
        obj_val_.clear();
    }
    obj_val_[key] = val;
}

bool JsonValue::contains(const std::string& key) const {
    if (type_ != JsonType::Object) return false;
    return obj_val_.find(key) != obj_val_.end();
}

const JsonValue& JsonValue::get(const std::string& key) const {
    static const JsonValue null_val;
    if (type_ != JsonType::Object) return null_val;
    auto it = obj_val_.find(key);
    return (it != obj_val_.end()) ? it->second : null_val;
}

const JsonValue& JsonValue::operator[](const std::string& key) const {
    return get(key);
}

JsonValue& JsonValue::operator[](const std::string& key) {
    if (type_ != JsonType::Object) {
        type_ = JsonType::Object;
        obj_val_.clear();
    }
    return obj_val_[key];
}

const std::map<std::string, JsonValue>& JsonValue::objectValues() const {
    static const std::map<std::string, JsonValue> empty_map;
    return (type_ == JsonType::Object) ? obj_val_ : empty_map;
}

static void escapeJsonString(const std::string& in, std::string& out) {
    out += '"';
    for (char c : in) {
        switch (c) {
            case '"': out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\b': out += "\\b"; break;
            case '\f': out += "\\f"; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 32) {
                    char buf[8];
                    snprintf(buf, sizeof(buf), "\\u%04x", c);
                    out += buf;
                } else {
                    out += c;
                }
                break;
        }
    }
    out += '"';
}

void JsonValue::serializeInternal(std::string& out, int indent, int current_indent) const {
    std::string nl = (indent > 0) ? "\n" : "";
    std::string space = (indent > 0) ? " " : "";
    std::string ind(current_indent, ' ');
    std::string next_ind(current_indent + indent, ' ');

    switch (type_) {
        case JsonType::Null:
            out += "null";
            break;
        case JsonType::Boolean:
            out += bool_val_ ? "true" : "false";
            break;
        case JsonType::Number: {
            char buf[64];
            if (num_val_ == static_cast<int64_t>(num_val_)) {
                snprintf(buf, sizeof(buf), "%lld", static_cast<long long>(num_val_));
            } else {
                snprintf(buf, sizeof(buf), "%.6f", num_val_);
                // Trim trailing zeroes
                size_t len = strlen(buf);
                while (len > 0 && buf[len - 1] == '0') {
                    buf[--len] = '\0';
                }
                if (len > 0 && buf[len - 1] == '.') {
                    buf[len - 1] = '\0';
                }
            }
            out += buf;
            break;
        }
        case JsonType::String:
            escapeJsonString(str_val_, out);
            break;
        case JsonType::Array: {
            if (arr_val_.empty()) {
                out += "[]";
                break;
            }
            out += "[" + nl;
            for (size_t i = 0; i < arr_val_.size(); ++i) {
                out += next_ind;
                arr_val_[i].serializeInternal(out, indent, current_indent + indent);
                if (i + 1 < arr_val_.size()) out += ",";
                out += nl;
            }
            out += ind + "]";
            break;
        }
        case JsonType::Object: {
            if (obj_val_.empty()) {
                out += "{}";
                break;
            }
            out += "{" + nl;
            size_t idx = 0;
            for (const auto& pair : obj_val_) {
                out += next_ind;
                escapeJsonString(pair.first, out);
                out += ":" + space;
                pair.second.serializeInternal(out, indent, current_indent + indent);
                if (++idx < obj_val_.size()) out += ",";
                out += nl;
            }
            out += ind + "}";
            break;
        }
    }
}

std::string JsonValue::serialize(int indent) const {
    std::string out;
    serializeInternal(out, indent, 0);
    return out;
}

namespace {

class JsonParser {
public:
    explicit JsonParser(std::string text) : src_(std::move(text)) {}

    JsonValue parse(std::string* err) {
        skipWhitespace();
        if (pos_ >= src_.size()) {
            if (err) *err = "Empty input";
            return JsonValue();
        }
        try {
            JsonValue val = parseValue();
            skipWhitespace();
            if (pos_ < src_.size()) {
                if (err) *err = "Unexpected trailing characters at position " + std::to_string(pos_);
            }
            return val;
        } catch (const std::exception& ex) {
            if (err) *err = ex.what();
            return JsonValue();
        }
    }

private:
    std::string src_;
    size_t pos_{0};

    void skipWhitespace() {
        while (pos_ < src_.size()) {
            char c = src_[pos_];
            if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
                pos_++;
            } else if (c == '/' && pos_ + 1 < src_.size() && src_[pos_ + 1] == '/') {
                // Line comment support
                pos_ += 2;
                while (pos_ < src_.size() && src_[pos_] != '\n') pos_++;
            } else {
                break;
            }
        }
    }

    char peek() {
        skipWhitespace();
        return (pos_ < src_.size()) ? src_[pos_] : '\0';
    }

    char get() {
        skipWhitespace();
        return (pos_ < src_.size()) ? src_[pos_++] : '\0';
    }

    JsonValue parseValue() {
        skipWhitespace();
        char c = peek();
        if (c == '{') return parseObject();
        if (c == '[') return parseArray();
        if (c == '"') return JsonValue(parseString());
        if (c == 't' || c == 'f') return JsonValue(parseBool());
        if (c == 'n') return parseNull();
        if (c == '-' || std::isdigit(static_cast<unsigned char>(c))) return parseNumber();

        throw std::runtime_error("Unexpected token '" + std::string(1, c) + "' at position " + std::to_string(pos_));
    }

    JsonValue parseObject() {
        get(); // '{'
        JsonValue obj(JsonType::Object);
        skipWhitespace();
        if (peek() == '}') {
            get();
            return obj;
        }

        while (true) {
            skipWhitespace();
            if (peek() != '"') {
                throw std::runtime_error("Expected string key in object at position " + std::to_string(pos_));
            }
            std::string key = parseString();
            skipWhitespace();
            if (get() != ':') {
                throw std::runtime_error("Expected ':' after key in object at position " + std::to_string(pos_));
            }
            JsonValue val = parseValue();
            obj.set(key, val);

            skipWhitespace();
            char next = peek();
            if (next == '}') {
                get();
                break;
            } else if (next == ',') {
                get();
            } else {
                throw std::runtime_error("Expected ',' or '}' in object at position " + std::to_string(pos_));
            }
        }
        return obj;
    }

    JsonValue parseArray() {
        get(); // '['
        JsonValue arr(JsonType::Array);
        skipWhitespace();
        if (peek() == ']') {
            get();
            return arr;
        }

        while (true) {
            arr.pushBack(parseValue());
            skipWhitespace();
            char next = peek();
            if (next == ']') {
                get();
                break;
            } else if (next == ',') {
                get();
            } else {
                throw std::runtime_error("Expected ',' or ']' in array at position " + std::to_string(pos_));
            }
        }
        return arr;
    }

    std::string parseString() {
        get(); // '"'
        std::string s;
        while (pos_ < src_.size()) {
            char c = src_[pos_++];
            if (c == '"') {
                return s;
            } else if (c == '\\') {
                if (pos_ >= src_.size()) throw std::runtime_error("Unterminated escape sequence");
                char esc = src_[pos_++];
                switch (esc) {
                    case '"': s += '"'; break;
                    case '\\': s += '\\'; break;
                    case '/': s += '/'; break;
                    case 'b': s += '\b'; break;
                    case 'f': s += '\f'; break;
                    case 'n': s += '\n'; break;
                    case 'r': s += '\r'; break;
                    case 't': s += '\t'; break;
                    case 'u': {
                        if (pos_ + 4 > src_.size()) throw std::runtime_error("Incomplete unicode escape");
                        pos_ += 4;
                        s += '?'; // simplified fallback for unicode codepoint
                        break;
                    }
                    default: s += esc; break;
                }
            } else {
                s += c;
            }
        }
        throw std::runtime_error("Unterminated string literal");
    }

    bool parseBool() {
        if (src_.compare(pos_, 4, "true") == 0) {
            pos_ += 4;
            return true;
        }
        if (src_.compare(pos_, 5, "false") == 0) {
            pos_ += 5;
            return false;
        }
        throw std::runtime_error("Invalid boolean literal at position " + std::to_string(pos_));
    }

    JsonValue parseNull() {
        if (src_.compare(pos_, 4, "null") == 0) {
            pos_ += 4;
            return JsonValue();
        }
        throw std::runtime_error("Invalid null literal at position " + std::to_string(pos_));
    }

    JsonValue parseNumber() {
        size_t start = pos_;
        if (src_[pos_] == '-') pos_++;
        while (pos_ < src_.size() && std::isdigit(static_cast<unsigned char>(src_[pos_]))) pos_++;
        if (pos_ < src_.size() && src_[pos_] == '.') {
            pos_++;
            while (pos_ < src_.size() && std::isdigit(static_cast<unsigned char>(src_[pos_]))) pos_++;
        }
        if (pos_ < src_.size() && (src_[pos_] == 'e' || src_[pos_] == 'E')) {
            pos_++;
            if (pos_ < src_.size() && (src_[pos_] == '+' || src_[pos_] == '-')) pos_++;
            while (pos_ < src_.size() && std::isdigit(static_cast<unsigned char>(src_[pos_]))) pos_++;
        }

        std::string num_str = src_.substr(start, pos_ - start);
        double val = std::stod(num_str);
        return JsonValue(val);
    }
};

} // namespace

JsonValue JsonValue::parse(const std::string& json_str, std::string* error_msg) {
    JsonParser parser(json_str);
    return parser.parse(error_msg);
}

} // namespace libreomr
