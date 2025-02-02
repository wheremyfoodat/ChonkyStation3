#include "Lv2Mutex.hpp"


bool Lv2Mutex::isFree() {
    return owner == -1;
}

void Lv2Mutex::lock(u32 owner) {
    // isFree should be called prior to this function to ensure the mutex is free
    this->owner = owner;
}

void Lv2Mutex::unlock() {
    owner = -1;
}