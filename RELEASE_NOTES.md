## LibreOMR 1.0.1

Patch release fixing bubble label matching in synthesized answer sheets.

### Bug Fixes

- Fixed substring collision in `Template::renderFilledSheet`: Answer label matching now uses exact token parsing (delimited by commas or semicolons) instead of substring search, preventing unintended fills on numeric or prefix-sharing labels (e.g., answering "10" no longer erroneously marks option "1").

### Assets and Distribution

Precompiled packages are available below for Linux (x86_64), Windows (x86_64), and macOS (Apple Silicon arm64). Integrity checksums are listed in `SHA256SUMS.txt`.
