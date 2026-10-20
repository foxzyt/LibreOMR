## LibreOMR 1.1.0

### Features & Fixes
- **Fiducial detection**: dynamic background contrast scoring and candidate search for desk backgrounds and high-DPI scans.
- **Binarization**: adaptive thresholding in processor pipeline for uneven lighting and shadows.
- **Orientation**: auto-orientation (0°, 90°, 180°, 270°) with contrast scoring for blank and filled sheets.
- **Alignment**: local bubble center refinement to compensate for curved paper.
- **Canvas**: expanded bounding box in `Image::rotate` to prevent corner clipping.

### API
- Added `auto_orient`, `refine_bubble_centers`, and adaptive thresholding options to C and C++ APIs.
