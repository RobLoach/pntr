# Changelog

All notable changes to *pntr* are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and *pntr* follows [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Fixed

- **`pntr_image_resize_canvas()` moves pixels rather than blending them** ([#269]). It used to draw the original onto the filled canvas, which composites: a semi-transparent pixel came out mixed with the fill, and under `PNTR_NO_ALPHABLEND` it was skipped entirely and the fill was left in its place, so a partially transparent image came back as a blank rectangle. The rows are copied now, which is independent of what the build does about blending.

  Resizing the canvas of an image with any non-opaque pixel in it previously changed those pixels; it no longer does. Measured on an image with a different alpha in every pixel: 30 of 30 wrong in the default build and 67 of 71 under `PNTR_NO_ALPHABLEND`, now 0 in both.

- **A coordinate near the end of the int range no longer overflows** ([#267]). `pntr_draw_rectangle_fill(image, INT_MAX - 1, 0, 10, 10, color)` segfaulted: the clip test computed `x + width`, which overflowed and wrapped negative, so the far edge came back smaller than the near one and the subtraction wrapped again into a positive width. An entirely off-image rectangle was reported as visible and written through. Squaring a radius or a thickness overflowed above 46340 as well, and the sum of two squares above about 32767, so a circle wide enough to cover the canvas was drawn from a wrapped radius.

  Measured over 30 entry points called with extreme but legal arguments: **21 reached undefined behavior, now 0.** Rendered output for ordinary values is unchanged, verified by hashing 20 primitives across 6 parameter variations against the previous `master`.

- **Bilinear interpolation rounds instead of truncating** ([#263]). Interpolating between four copies of the same color now returns that color exactly, on every channel and at every fractional coordinate. The four weights sum to one in exact arithmetic but not in float, so truncating the result shaved a unit off: a bilinear-scaled draw of a fully opaque image produced pixels at an alpha of 254, which `PNTR_NO_ALPHABLEND` then discarded outright and left holes in the fill. Red, green and blue were losing the same unit in the same places.

  **What to do:** rendered output changes by at most one unit per channel for anything scaled, resized or rotated with `PNTR_FILTER_BILINEAR`, so pixel-exact baselines against bilinear output need regenerating. `pntr_draw_image_dest_rec()` with a bilinear filter now fills its destination rectangle completely under `PNTR_NO_ALPHABLEND`, matching nearest neighbor.

## [0.1.0] - 2026-10-12

The first tagged release of *pntr*. Since there was no previous tag, the entries below are written against the untagged `master` history, so they are aimed at anyone who has been vendoring `pntr.h` directly or shipping one of the integrations.

### Breaking Changes

- **The drawing bounds convention was settled** ([#245], [#232]). Whether a shape covers its far edge now depends on how the shape is described, and every primitive follows the same rule:
  - **Point-valued parameters and radii are inclusive.** `pntr_draw_line(image, 0, 0, 10, 0, color)` paints 11 pixels, `x = 0` through `x = 10`. `pntr_draw_circle_fill(image, 50, 50, 10, color)` is 21 pixels across, and covers exactly the pixels that `pntr_draw_circle()` outlines for the same center and radius. The same holds for `pntr_draw_line_aa()`, `pntr_draw_line_thick()`, `pntr_draw_line_curve()`, `pntr_draw_polyline()`, `pntr_draw_polygon()`, `pntr_draw_triangle()`, their fills, and the ellipses.
  - **Extent-valued parameters stop short of the far edge.** `pntr_draw_rectangle(image, x, y, width, height, color)` paints columns `x` through `x + width - 1`, so `width` and `height` are pixel counts, not second coordinates. `pntr_draw_rectangle_fill()`, `pntr_draw_rectangle_rounded()`, `pntr_draw_line_horizontal()`, `pntr_draw_line_vertical()` and `pntr_image_set_clip()` read their sizes the same way.

  **What to do:** rendered output changed for lines, polygon and triangle fills, filled circles and ellipses, rounded rectangles, and line thicknesses 2 and 3. Nothing in the API signatures changed, so nothing stops compiling, but anything doing a pixel-exact comparison against *pntr* output (reference screenshots, golden-image tests, hashes) needs its baselines regenerated. A rounded rectangle now occupies the same pixels as a plain rectangle with the same `x`, `y`, `width` and `height`, and a thick line is the same length as the thin line through the same two endpoints. The full rule is written out under [Bounds](README.md#bounds).

- **The rotation offset became a pivot** ([#249], [#255]). `offsetX`/`offsetY` on `pntr_draw_image_rotated()` and `pntr_draw_image_rotated_rec()`, and `originX`/`originY` on `pntr_draw_image_rotozoom()`, are now a pivot expressed in unrotated, unscaled source pixels: the source pixel at that offset lands on `posX`, `posY`, at every angle. Previously an offset of `0, 0` effectively placed the rotated bounding box rather than a source pixel.

  **What to do:** any call passing `0, 0` with a non-zero rotation will see its sprite move. To rotate a sprite about its center, pass the center of the source in source pixels, `(width - 1) / 2.0f` and `(height - 1) / 2.0f`; to pin a sprite by its top-left source pixel, `0, 0` is now correct and stays correct as the sprite turns. This reaches the integrations that draw rotated sprites, including [pntr_app], [pntr_aseprite] and [pntr_tiled]. Rotozoom is now continuous through a rotation of zero and through the quarter turns, so sweeping an angle no longer jumps by a pixel as it crosses 0, 90, 180 or 270 degrees ([#249], [#255]).

### Added

- `pntr_image_get_color_bilinear()`, for sampling an image at a floating point coordinate with bilinear interpolation ([#231]).
- `pntr_draw_image_dest_rec()`, for drawing a source rectangle into an arbitrary destination rectangle, scaling and tinting to fit.
- `PNTR_VERSION`, `PNTR_VERSION_MAJOR`, `PNTR_VERSION_MINOR` and `PNTR_VERSION_PATCH`, so a consumer can report or `#if` on the version of `pntr.h` it was built against.
- `PNTR_ERROR_FAILED_TO_READ` and `PNTR_ERROR_INVALID_DATA`, which distinguish a file that opened but could not be read from data that is not what it claimed to be ([#242]).
- `PNTR_ERROR_STORAGE`, the storage class holding the last reported error. It defaults to `static`, which every thread shares; define it as a thread-local storage class, like `static _Thread_local`, to give each thread an error of its own ([#242]).
- `PNTR_NO_BMP`, to drop BMP loading and save roughly 6.5 KB. BMP saving is unaffected.
- `pntr_draw_image_rotozoom()`, which rotates, scales and tints a source rectangle in one pass ([#121], [#219]).
- A `tint` parameter on `pntr_draw_image_scaled_rec()` ([#217]).
- `pntr_draw_line_aa()`, for anti-aliased lines ([#212]).
- `pntr_color_grayscale()` and `pntr_image_color_grayscale()` ([#214]).
- `pntr_draw_text_aligned()`, with `PNTR_TEXT_ALIGN_LEFT`, `CENTER` and `RIGHT` ([#215]).
- A thickness variant for every primitive: `pntr_draw_line_thick()`, `pntr_draw_line_thick_vec()`, `pntr_draw_line_horizontal_thick()`, `pntr_draw_line_vertical_thick()`, `pntr_draw_line_curve_thick()`, `pntr_draw_rectangle_thick()`, `pntr_draw_rectangle_thick_rec()`, `pntr_draw_rectangle_thick_rounded()`, `pntr_draw_circle_thick()`, `pntr_draw_ellipse_thick()`, `pntr_draw_triangle_thick()`, `pntr_draw_triangle_thick_vec()`, `pntr_draw_arc_thick()`, `pntr_draw_polygon_thick()` and `pntr_draw_polyline_thick()`.
- Documentation for every public function, parameter, macro and configuration define, published at <https://robloach.github.io/pntr/>. The docs build now fails on Doxygen warnings, and no longer needs Graphviz ([#251]).

### Changed

- BMP loading is now enabled by default, matching the formats *pntr* can save. Opt out with `PNTR_NO_BMP`. JPEG loading still needs `PNTR_ENABLE_JPEG`, costing roughly 21 KB.
- `pntr_load_image()` no longer masks the underlying file error with a generic one, so a missing file, an unreadable file and a corrupt image are now told apart through `pntr_get_error()` and `pntr_get_error_code()` ([#242]).
- Failure paths that used to return `NULL` or `false` silently now report an error, so `pntr_get_error()` is meaningful after any failed call ([#242]).
- Internal symbols have internal linkage, so including the implementation in a translation unit no longer leaks private helpers into the link.
- `PNTR_NO_LOAD_IMAGE` and `PNTR_NO_SAVE_IMAGE` are honored throughout, instead of being partially ignored, and a disabled save reports a size of zero rather than leaving the caller's size untouched.
- The smallest circles and ellipses are drawn at their own radius, so a radius of one is three pixels across, a radius of zero is the center pixel on its own, and an ellipse with a zero radius on one axis is the straight line through its center ([#250]).
- Every pixel of a primitive is blended exactly once, so overlapping spans in a single shape no longer double-blend a translucent color.
- The committed example screenshots were regenerated against the settled conventions.

### Fixed

- Heap overflow in `pntr_draw_arc_fill()` ([#236]).
- Heap overflow in `pntr_clear_background()` when called on a subimage ([#237]).
- Out-of-bounds reads in bilinear filtering. Every bilinear sample now goes through one clamped sampler, which also stops `pntr_image_resize()` from reading past the right and bottom edges, and stops the rotated and rotozoom draws from skipping the last row and column of the source instead of sampling it ([#231]). Bilinear scaling of a source rectangle also read its X from the rectangle's Y offset ([#219]).
- Out-of-bounds writes and an off-by-one in `pntr_draw_image_rotated_rec()` ([#238]).
- The font loaders are hardened against truncated and malformed font data, instead of trusting the glyph counts and offsets they are given ([#240]).
- Buffer overflow when formatting an assertion failure message, and sharper assertion diagnostics ([#239], [#230]).
- `PNTR_ASSERT_LESSEREQ()` and `PNTR_ASSERT_GREATEREQ()` were inverted ([#241]).
- BMP and JPEG saving now produce valid files. The write callback kept only the last chunk, and both encoders stream their output in many small pieces, so anything other than PNG came out truncated.
- Signed shift undefined behaviour in the JPEG encoder ([#241]).
- `pntr_image_to_pixelformat()` and `pntr_image_from_pixelformat()` round-trip correctly in `PNTR_PIXELFORMAT_ARGB` builds, which previously swapped components on the way back.
- Allocation size arithmetic can no longer overflow before the allocation is made.
- `pntr_image_copy()` is an exact copy. It used to blend the source onto a blank image, which was lossy for partial alpha and dropped every pixel under `PNTR_NO_ALPHABLEND`. Blending onto a fully transparent destination now yields the source exactly, instead of an alpha one unit too high.
- Mask offset in `pntr_image_alpha_mask()`.
- The contrast formula in `pntr_color_contrast()`, and clipping in `pntr_image_color_replace()`.
- Infinite loop in the built-in `sqrtf` when given zero, in `PNTR_NO_MATH` builds.
- `pntr_image_resize_canvas()` preserves a custom clip rectangle ([#213]).
- Ellipse sizing, by way of a two-region midpoint algorithm ([#211]).
- The UTF-8 codepoint helper returns early on `NULL` instead of dereferencing it.
- Defining `PNTR_NO_CUTE_PNG_IMPLEMENTATION` or one of the `PNTR_NO_STB_*_IMPLEMENTATION` defines yourself no longer trips a macro redefinition warning, and the `PNTR_CUTE_PNG` backend reports a size of zero when saving fails ([#241]).
- Unused parameter warnings in the disabled-feature stubs, so a build with loading or saving turned off is warning-clean.
- `vsnprintf()` is used when formatting text to prevent buffer overflows (thanks [@konsumer]).
- An off-by-one error in image drawing (thanks [@JesseTG]).

[#121]: https://github.com/RobLoach/pntr/pull/121
[#211]: https://github.com/RobLoach/pntr/pull/211
[#212]: https://github.com/RobLoach/pntr/pull/212
[#214]: https://github.com/RobLoach/pntr/pull/214
[#213]: https://github.com/RobLoach/pntr/pull/213
[#215]: https://github.com/RobLoach/pntr/pull/215
[#217]: https://github.com/RobLoach/pntr/pull/217
[#219]: https://github.com/RobLoach/pntr/pull/219
[#230]: https://github.com/RobLoach/pntr/pull/230
[#231]: https://github.com/RobLoach/pntr/pull/231
[#232]: https://github.com/RobLoach/pntr/issues/232
[#236]: https://github.com/RobLoach/pntr/pull/236
[#237]: https://github.com/RobLoach/pntr/pull/237
[#238]: https://github.com/RobLoach/pntr/pull/238
[#239]: https://github.com/RobLoach/pntr/pull/239
[#240]: https://github.com/RobLoach/pntr/pull/240
[#241]: https://github.com/RobLoach/pntr/pull/241
[#242]: https://github.com/RobLoach/pntr/pull/242
[#245]: https://github.com/RobLoach/pntr/pull/245
[#249]: https://github.com/RobLoach/pntr/pull/249
[#250]: https://github.com/RobLoach/pntr/pull/250
[#251]: https://github.com/RobLoach/pntr/pull/251
[#255]: https://github.com/RobLoach/pntr/pull/255
[@JesseTG]: https://github.com/JesseTG
[@konsumer]: https://github.com/konsumer
[pntr_app]: https://github.com/RobLoach/pntr_app
[pntr_aseprite]: https://github.com/RobLoach/pntr_aseprite
[pntr_tiled]: https://github.com/RobLoach/pntr_tiled
[Unreleased]: https://github.com/RobLoach/pntr/compare/v0.1.0...HEAD
[0.1.0]: https://github.com/RobLoach/pntr/releases/tag/v0.1.0
[#263]: https://github.com/RobLoach/pntr/issues/263
[#267]: https://github.com/RobLoach/pntr/pull/267
[#269]: https://github.com/RobLoach/pntr/issues/269
