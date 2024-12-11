#pragma once

#include <common.hpp>
#include <functional>
#include <CellTypes.hpp>


using namespace CellTypes;

class Import {
public:
    Import() {}
    Import(std::string name, std::function<Result(void)> const& handler) : name(name), handler(handler) {}
    std::string name;
    std::function<Result(void)> handler;
};