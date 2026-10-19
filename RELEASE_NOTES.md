## LibreOMR 1.0.3

Patch release resolving coordinate space projection in debug rendering and adding defensive allocation guards.

### Bug Fixes

- **Debug Overlay Projection**: Fixed coordinate space mismatch in `PageResult::renderDebugImage` and `libreomr_result_render_debug` when processing rotated or skewed images. Bubble overlays are now projected via template-to-base homography directly onto the original image space.
- **Memory Safety**: Added `nullptr` guards post-`calloc` in `PageResult::toC` across questions and bubble allocations to prevent null pointer dereferences on out-of-memory (OOM) conditions.

### Assets and Distribution

Precompiled packages are available below for Linux (x86_64), Windows (x86_64), and macOS (Apple Silicon arm64). Integrity checksums are listed in `SHA256SUMS.txt`.
