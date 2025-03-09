#pragma once

#include <common.hpp>
#include <BEField.hpp>


namespace sys_event {

struct sys_event {
    BEField<u64> source;
    BEField<u64> data1;
    BEField<u64> data2;
    BEField<u64> data3;
};

}   // End namespace sys_event