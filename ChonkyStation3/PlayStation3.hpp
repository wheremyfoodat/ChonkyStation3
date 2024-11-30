#pragma once

#include <common.hpp>
#include <ELF/ELFloader.hpp>
#include <Memory.hpp>


class PlayStation3 {
public:
	PlayStation3(const fs::path& executable);
	Memory mem;
};