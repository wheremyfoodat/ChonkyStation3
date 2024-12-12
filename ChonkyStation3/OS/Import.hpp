#pragma once

#include <common.hpp>
#include <functional>
#include <CellTypes.hpp>


using namespace CellTypes;

class Import {
public:
    Import() {}
    Import(std::string name, std::function<u64(void)> const& handler) : name(name), handler(handler) {}
    std::string name;
    std::function<u64(void)> handler;
};