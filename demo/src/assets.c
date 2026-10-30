// The archive bytes are part of the executable, assetsinc is generated from
// assets.zip by the Makefile (boxer tools/bin2c)
#include "assets.h"

const unsigned char assets_data[] = {
#include "assets.inc"
};

const size_t assets_length = sizeof(assets_data);
