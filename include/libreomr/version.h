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

#ifndef LIBREOMR_VERSION_H
#define LIBREOMR_VERSION_H

#include "libreomr/export.h"

#define LIBREOMR_VERSION_MAJOR 1
#define LIBREOMR_VERSION_MINOR 0
#define LIBREOMR_VERSION_PATCH 3
#define LIBREOMR_VERSION_STRING "1.0.3"

#ifdef __cplusplus
extern "C" {
#endif

LIBREOMR_API int LIBREOMR_CALL libreomr_version_major(void);
LIBREOMR_API int LIBREOMR_CALL libreomr_version_minor(void);
LIBREOMR_API int LIBREOMR_CALL libreomr_version_patch(void);
LIBREOMR_API const char* LIBREOMR_CALL libreomr_version_string(void);

#ifdef __cplusplus
}
#endif

#endif /* LIBREOMR_VERSION_H */
