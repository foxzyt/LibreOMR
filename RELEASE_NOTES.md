## LibreOMR 1.0.2

Patch release fixing out-of-bounds crash on empty input images.

### Bug Fixes

- Fixed unhandled exception in `Processor::process`: Processing empty images (0x0 or unallocated buffers) or zero-dimension templates now returns a graceful failed-alignment `PageResult` instead of throwing `std::out_of_range`.

### Assets and Distribution

Precompiled packages are available below for Linux (x86_64), Windows (x86_64), and macOS (Apple Silicon arm64). Integrity checksums are listed in `SHA256SUMS.txt`.
