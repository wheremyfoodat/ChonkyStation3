#pragma once

#include <common.hpp>


template<typename T>
struct BEField {
public:
	T val;

	operator T() {
		return Helpers::bswap<T>(val);
	}
};