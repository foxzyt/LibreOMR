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

#include "libreomr/version.h"

extern "C" {

int libreomr_version_major(void) {
    return LIBREOMR_VERSION_MAJOR;
}

int libreomr_version_minor(void) {
    return LIBREOMR_VERSION_MINOR;
}

int libreomr_version_patch(void) {
    return LIBREOMR_VERSION_PATCH;
}

const char* libreomr_version_string(void) {
    return LIBREOMR_VERSION_STRING;
}

}
