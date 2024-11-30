#pragma once

#include <common.hpp>


template<typename T>
struct BEField {
public:
	T val;
	/*BEField() {}
	
	template<typename T>
	T operator=(T val) {
		this->val = Helpers::bswap<T>(val);
		return val;
	}*/

	operator T() {
		return Helpers::bswap<T>(val);
	}
};