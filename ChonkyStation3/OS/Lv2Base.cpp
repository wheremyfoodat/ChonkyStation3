#include "Lv2Base.hpp"
#include "Lv2Object.hpp"


u64 Lv2Base::handle() {
    return obj->handle;
}

PlayStation3* Lv2Base::ps3() {
    return obj->ps3;
}