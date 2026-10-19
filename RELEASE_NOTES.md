## LibreOMR 1.0.0

Initial release of LibreOMR, a C++17 and C library for Optical Mark Recognition (OMR).

### Core Features

- Document rectification: Automatic detection of 4 corner registration fiducials via connected component analysis and projective homography computation using Hartley-normalized Direct Linear Transformation (DLT).
- Photometric analysis: Otsu optimal global thresholding, integral-image adaptive local thresholding, and differential inner-bubble sampling relative to local paper background.
- Classification and ambiguity handling: Fill ratio calculation, relative contrast scoring, detection of multiple marks on single-choice fields, and confidence interval estimation.
- Dual API:
  - Modern C++ API (`libreomr/libreomr.hpp`) with RAII, STL containers, and integrated JSON serialization.
  - C ABI (`libreomr/libreomr.h`) with opaque handles, explicit error codes, thread-local error retrieval, and dynamic/static linking support.
- Zero external image or parser dependencies: Integrated PNG, JPEG, BMP, and TGA codecs, and embedded JSON parser/serializer.
- Form synthesis: Programmatic rendering of scannable blank forms and calibration answer sheets directly from template definitions.
- CLI utility (`libreomr`): Command-line tool for template creation, sheet rendering, batch form evaluation, and debug visual overlay export.

### Assets and Distribution

Precompiled packages are available below for Linux (x86_64), Windows (x86_64), and macOS (Apple Silicon arm64). Integrity checksums are listed in `SHA256SUMS.txt`.
