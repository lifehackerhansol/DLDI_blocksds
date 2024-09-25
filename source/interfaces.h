/*
	Copyright (C) 2023 lifehackerhansol

	SPDX-License-Identifier: Zlib
*/

#pragma once

#if defined(PLATFORM_r4tf)
#include "platform/r4tf/interface.h"
#elif defined(PLATFORM_acep)
#include "platform/acep/interface.h"
#else
#error "You must select a platform!"
#endif

