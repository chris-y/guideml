#pragma once

#include <stdint.h>
#include <stdlib.h>

static inline uint16_t RangeRand(uint16_t limit)
{
	return random() % limit;
}
