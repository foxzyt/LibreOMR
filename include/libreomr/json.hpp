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

#ifndef LIBREOMR_JSON_HPP
#define LIBREOMR_JSON_HPP

#include "libreomr/export.h"
#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace libreomr {

enum class JsonType {
    Null,
    Boolean,
    Number,
    String,
    Array,
    Object
};

class LIBREOMR_API JsonValue {
public:
    JsonValue();
    explicit JsonValue(bool val);
    explicit JsonValue(double val);
    explicit JsonValue(int val);
    explicit JsonValue(uint32_t val);
    explicit JsonValue(const char* val);
    explicit JsonValue(std::string val);
    explicit JsonValue(JsonType type);

    [[nodiscard]] JsonType type() const noexcept { return type_; }
    [[nodiscard]] bool isNull() const noexcept { return type_ == JsonType::Null; }
    [[nodiscard]] bool isBool() const noexcept { return type_ == JsonType::Boolean; }
    [[nodiscard]] bool isNumber() const noexcept { return type_ == JsonType::Number; }
    [[nodiscard]] bool isString() const noexcept { return type_ == JsonType::String; }
    [[nodiscard]] bool isArray() const noexcept { return type_ == JsonType::Array; }
    [[nodiscard]] bool isObject() const noexcept { return type_ == JsonType::Object; }

    [[nodiscard]] bool asBool(bool def = false) const;
    [[nodiscard]] double asDouble(double def = 0.0) const;
    [[nodiscard]] int asInt(int def = 0) const;
    [[nodiscard]] uint32_t asUint(uint32_t def = 0) const;
    [[nodiscard]] const std::string& asString() const;

    // Array operations
    void pushBack(const JsonValue& val);
    [[nodiscard]] size_t size() const;
    [[nodiscard]] const JsonValue& operator[](size_t index) const;
    JsonValue& operator[](size_t index);
    [[nodiscard]] const std::vector<JsonValue>& arrayValues() const;

    // Object operations
    void set(const std::string& key, const JsonValue& val);
    [[nodiscard]] bool contains(const std::string& key) const;
    [[nodiscard]] const JsonValue& get(const std::string& key) const;
    [[nodiscard]] const JsonValue& operator[](const std::string& key) const;
    JsonValue& operator[](const std::string& key);
    [[nodiscard]] const std::map<std::string, JsonValue>& objectValues() const;

    [[nodiscard]] std::string serialize(int indent = 2) const;
    static JsonValue parse(const std::string& json_str, std::string* error_msg = nullptr);

private:
    JsonType type_{JsonType::Null};
    bool bool_val_{false};
    double num_val_{0.0};
    std::string str_val_{};
    std::vector<JsonValue> arr_val_{};
    std::map<std::string, JsonValue> obj_val_{};

    void serializeInternal(std::string& out, int indent, int current_indent) const;
};

} // namespace libreomr

#endif /* LIBREOMR_JSON_HPP */
