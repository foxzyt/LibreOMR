## LibreOMR 1.1.0

### Changes

- **Fiducial Detection**: Support corner detection when non-page background (e.g. table margins) is present around the sheet using local contrast scoring and fallback candidate search.
- **Binarization**: Use adaptive thresholding instead of global Otsu in `Processor::process` to handle non-uniform lighting and shadows.
- **Bubble Alignment**: Add `refine_bubble_centers` option to locally adjust bubble sampling coordinates based on ink centroid, compensating for paper curvature.
- **Orientation**: Add `auto_orient` to detect 0°, 90°, 180°, and 270° page rotations using candidate quad scoring.
- **Image Rotation**: Update `Image::rotate` to expand canvas dimensions to prevent clipping rotated content.

### API Additions

- C++: `ProcessorConfig::auto_orient`, `ProcessorConfig::refine_bubble_centers`, and corresponding getters/setters in `Processor`.
- C API: `libreomr_processor_set_auto_orient`, `libreomr_processor_set_adaptive_thresholding`, `libreomr_processor_set_refine_bubble_centers`.

### Distribution

Precompiled packages and `SHA256SUMS.txt` are attached below.
