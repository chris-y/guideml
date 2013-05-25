#pragma once
#include <ByteOrder.h>

static inline long BE_SWAPLONG(long in) {
	return B_LENDIAN_TO_HOST_INT32(in);
}

static inline long LE_SWAPLONG(long in) {
	return B_BENDIAN_TO_HOST_INT32(in);
}

static inline short LE_SWAPWORD(short in) {
	return B_BENDIAN_TO_HOST_INT16(in);
}
