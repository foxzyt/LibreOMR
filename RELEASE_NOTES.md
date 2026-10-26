## LibreOMR 1.2.0

### Desktop Application & GUI
- **LibreOMR Desktop**: Built-in desktop graphical interface powered by native WebView2 for instant single-sheet grading and high-throughput batch evaluation.
- **Custom Question Weights & Void Questions**: Full support for weighted grading (custom point values per question) and voided/canceled questions (*anuladas*) automatically awarding full credit.
- **Student Roster Integration**: Automatic enrollment and student name recognition, roster mapping via CSV import, and per-student performance breakdown.
- **Statistical KPI Dashboard & Item Analysis**: Real-time batch metrics (class average, approval rate, highest and lowest scores) alongside automated question difficulty classification (Easy / Medium / Hard) and option frequency distribution.
- **Canonical A4 Answer Sheet Generator**: High-precision vector and bitmap sheet generation (PDF/SVG/PNG) matching the OMR recognition template 1:1, complete with corner fiducial markers and multilingual headers (PT, EN, ES).
- **Full Internationalization (i18n)**: Native UI translation covering Portuguese (BR), English (US), and Spanish (ES).
- **Native Windows Shell Integration**: Embedded multi-resolution application icon (`app.ico`) in the executable PE resource table and Windows taskbar window binding (`WM_SETICON`).

### Core Library & CLI
- **Template Rendering**: Added `Template::renderBlankSheet` supporting customizable DPI and multilingual labels.
- **CLI Updates**: Added commands and flags for blank answer sheet generation and refined bubble analysis options.
