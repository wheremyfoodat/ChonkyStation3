#include "SPUThread.hpp"
#include "PlayStation3.hpp"


SPUThread::SPUThread(PlayStation3* ps3, std::string name) : ps3(ps3) {
    id = ps3->handle_manager.request();
    this->name = name;
    
    log("Created SPU thread %d \"%s\"\n", id, name.c_str());
}