/*
    Copyright (C) 2023 lifehackerhansol

    SPDX-License-Identifier: Zlib
*/

#pragma once

#if defined(PLATFORM_r4tf)
#include "platform/r4tf/interface.h"
#elif defined(PLATFORM_acep)
#include "platform/acep/interface.h"
#elif defined(PLATFORM_ak2)
#include "platform/ak2/interface.h"
#elif defined(PLATFORM_g003)
#include "platform/g003/interface.h"
#elif defined(PLATFORM_iply)
#include "platform/iply/interface.h"
#elif defined(PLATFORM_m3ds)
#include "platform/m3ds/interface.h"
#elif defined(PLATFORM_r4idsn)
#include "platform/r4idsn/interface.h"
#elif defined(PLATFORM_rpgs)
#include "platform/rpgs/interface.h"
#elif defined(PLATFORM_scds)
#include "platform/scds/interface.h"
#elif defined(PLATFORM_scdssdhc)
#include "platform/scdssdhc/interface.h"
#elif defined(PLATFORM_sg3d)
#include "platform/sg3d/interface.h"
#elif defined(PLATFORM_ttio)
#include "platform/ttio/interface.h"
#else
#error "You must select a platform!"
#endif
