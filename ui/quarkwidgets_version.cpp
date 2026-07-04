/**
 * @file quarkwidgets_version.cpp
 * @brief Runtime ABI version query for libquarkwidgets.so.
 *
 * Returns the version baked into @ref QUARKWIDGETS_ABI_VERSION at the .so's
 * build time. Consumers compare this against their compile-time version to
 * detect deploy-time .so swaps.
 *
 * @copyright Copyright (c) 2026
 */
#include "quarkwidgets_version.h"

const char* quarkwidgets_runtime_abi_version() {
    return QUARKWIDGETS_ABI_VERSION;
}
