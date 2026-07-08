/**
 * @file quarkwidgets_version.h
 * @brief QuarkWidgets ABI version (compile-time + runtime query).
 *
 * Compile-time: @ref QUARKWIDGETS_ABI_VERSION is baked into every consumer
 * that includes this header (e.g. mini-apps executables).
 *
 * Runtime: @ref quarkwidgets_runtime_abi_version is resolved from the loaded
 * libquarkwidgets.so and reports the .so's build-time version.
 *
 * A mismatch between the two means a deploy swapped in an incompatible
 * libquarkwidgets.so — consumers should fail fast (qFatal) rather than risk
 * ABI UB. See mini-apps plan §ABI 防护 (运行时自检).
 *
 * @note Bump QUARKWIDGETS_ABI_VERSION in lockstep with the QuarkWidgets git
 * tag (project VERSION in CMakeLists.txt).
 *
 * @copyright Copyright (c) 2026
 */
#pragma once

#include "export.h"  // QW_EXPORT

/** Compile-time ABI version string (baked into consumers at #include time). */
#define QUARKWIDGETS_ABI_VERSION "0.1.0"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Runtime ABI version of the loaded libquarkwidgets.so.
 * @return Build-time version string of the .so actually loaded.
 *
 * Compare against @ref QUARKWIDGETS_ABI_VERSION (compile-time) to detect a
 * deploy that swapped an incompatible .so.
 */
QW_EXPORT const char* quarkwidgets_runtime_abi_version();

#ifdef __cplusplus
}  // extern "C"
#endif
