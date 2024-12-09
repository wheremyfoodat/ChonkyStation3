#pragma once

#include <common.hpp>
#include <unordered_map>


// Circular dependency
class PlayStation3;

class Module {
protected:
    Module(PlayStation3* ps3) : ps3(ps3) {}
    PlayStation3* ps3;
};