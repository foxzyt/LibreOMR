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

#ifndef LIBREOMR_EXPORT_H
#define LIBREOMR_EXPORT_H

#if defined(_WIN32) || defined(__CYGWIN__)
    #if defined(LIBREOMR_STATIC_DEFINE)
        #define LIBREOMR_API
    #elif defined(LIBREOMR_EXPORTS)
        #define LIBREOMR_API __declspec(dllexport)
    #else
        #define LIBREOMR_API __declspec(dllimport)
    #endif
    #define LIBREOMR_CALL __cdecl
#else
    #if defined(__GNUC__) && __GNUC__ >= 4
        #define LIBREOMR_API __attribute__((visibility("default")))
    #else
        #define LIBREOMR_API
    #endif
    #define LIBREOMR_CALL
#endif

#endif /* LIBREOMR_EXPORT_H */
