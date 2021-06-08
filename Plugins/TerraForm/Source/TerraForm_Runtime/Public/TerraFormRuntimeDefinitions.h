#pragma once
#include "TerraForm_Runtime/Public/TerraFormTypes.h"

#ifdef TFCONFIG
#define TFLicense ETerraFormLicense(TFCONFIG)
#else
#define TFLicense ETerraFormLicense::TL_Pro
#endif

#define TFRelease 2
#define TFVersionMajor 1
#define TFVersionMinor 5
#define TFVersionInternal 1

#define TFATTCONST FString("Value")