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

#ifndef LIBREOMR_TEST_FRAMEWORK_HPP
#define LIBREOMR_TEST_FRAMEWORK_HPP

#include <cmath>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

namespace libreomr::test {

struct TestCase {
    std::string name;
    std::function<void()> func;
};

class TestRunner {
public:
    static TestRunner& instance() {
        static TestRunner runner;
        return runner;
    }

    void registerTest(const std::string& name, std::function<void()> func) {
        tests_.push_back({name, std::move(func)});
    }

    int run() {
        std::cout << "\n========================================\n";
        std::cout << "  Running LibreOMR Unit Test Suite      \n";
        std::cout << "========================================\n\n";

        int passed = 0;
        int failed = 0;

        for (const auto& test : tests_) {
            std::cout << "[RUN ] " << test.name << "..." << std::flush;
            try {
                test.func();
                std::cout << " [PASS]\n";
                passed++;
            } catch (const std::exception& ex) {
                std::cout << " [FAIL] -> " << ex.what() << "\n";
                failed++;
            } catch (...) {
                std::cout << " [FAIL] -> Unknown exception\n";
                failed++;
            }
        }

        std::cout << "\n----------------------------------------\n";
        std::cout << "Total: " << tests_.size() << " | Passed: " << passed
                  << " | Failed: " << failed << "\n";
        std::cout << "========================================\n\n";

        return failed == 0 ? 0 : 1;
    }

private:
    std::vector<TestCase> tests_;
};

struct TestRegistrar {
    TestRegistrar(const std::string& name, std::function<void()> func) {
        TestRunner::instance().registerTest(name, std::move(func));
    }
};

#define TEST_CASE(name) \
    static void name(); \
    static ::libreomr::test::TestRegistrar registrar_##name(#name, name); \
    static void name()

#define ASSERT_TRUE(cond) \
    do { \
        if (!(cond)) { \
            throw std::runtime_error(std::string("Assertion failed: ") + #cond + " at " + __FILE__ + ":" + std::to_string(__LINE__)); \
        } \
    } while (0)

#define ASSERT_FALSE(cond) ASSERT_TRUE(!(cond))

#define ASSERT_EQ(a, b) \
    do { \
        if (!((a) == (b))) { \
            throw std::runtime_error(std::string("Assertion failed: ") + #a + " == " + #b + " at " + __FILE__ + ":" + std::to_string(__LINE__)); \
        } \
    } while (0)

#define ASSERT_NEAR(a, b, eps) \
    do { \
        if (std::abs((a) - (b)) > (eps)) { \
            throw std::runtime_error(std::string("Assertion failed: |") + #a + " - " + #b + "| <= " + #eps + " at " + __FILE__ + ":" + std::to_string(__LINE__)); \
        } \
    } while (0)

} // namespace libreomr::test

#endif /* LIBREOMR_TEST_FRAMEWORK_HPP */
