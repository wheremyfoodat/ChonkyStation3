#include "Lv2SPUThreadGroup.hpp"
#include "PlayStation3.hpp"


std::string Lv2SPUThreadGroup::getName(PlayStation3* ps3) {
    return Helpers::readString(ps3->mem.getPtr(attr->name_ptr));
}