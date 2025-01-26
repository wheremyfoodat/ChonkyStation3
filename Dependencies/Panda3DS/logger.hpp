#pragma once
#include <cstdarg>
#include <fstream>


namespace Log {
// Our logger class
template <bool enabled>
class Logger {
public:
    Logger(std::string prefix = "") : prefix(prefix) {}

    std::string prefix;

    void log(const char* fmt, ...) {
        if constexpr (!enabled) return;

        std::fputs(prefix.c_str(), stdout);
        std::va_list args;
        va_start(args, fmt);
        std::vprintf(fmt, args);
        va_end(args);
    }

    void logNoPrefix(const char* fmt, ...) {
        if constexpr (!enabled) return;

        std::va_list args;
        va_start(args, fmt);
        std::vprintf(fmt, args);
        va_end(args);
    }
};

// Our loggers here. Enable/disable by toggling the template param

#define true false
//#define false true

// Modules
static Logger sceNpTrophy       = Logger<true> ("[Module ][sceNpTrophy   ] ");
static Logger cellPngDec        = Logger<true> ("[Module ][cellPngDec    ] ");
static Logger cellFs            = Logger<true> ("[Module ][cellFs        ] ");
static Logger cellRtc           = Logger<true> ("[Module ][cellRtc       ] ");
static Logger cellSpurs         = Logger<true> ("[Module ][cellSpurs     ] ");
static Logger cellGame          = Logger<true> ("[Module ][cellGame      ] ");
static Logger cellResc          = Logger<false>("[Module ][cellResc      ] ");
static Logger cellSysmodule     = Logger<true> ("[Module ][cellSysmodule ] ");
static Logger cellSysutil       = Logger<true> ("[Module ][cellSysutil   ] ");
static Logger cellGcmSys        = Logger<true> ("[Module ][cellGcmSys    ] ");
static Logger cellVideoOut      = Logger<true> ("[Module ][cellVideoOut  ] ");
static Logger sysLwMutex        = Logger<false>("[Module ][sysLwMutex    ] ");
static Logger sysMMapper        = Logger<true> ("[Module ][sysMMapper    ] ");
static Logger sysPrxForUser     = Logger<false>("[Module ][sysPrxForUser ] ");
static Logger sysThread         = Logger<true> ("[Module ][sysThread     ] ");

// Syscalls
static Logger sysEvent          = Logger<true> ("[Syscall][sysEvent      ] ");
static Logger sysMemory         = Logger<true> ("[Syscall][sysMemory     ] ");
static Logger sysMMapper_sc     = Logger<true> ("[Syscall][sysMMapper    ] ");
static Logger sysTimer          = Logger<false>("[Syscall][sysTimer      ] ");
static Logger misc_sc           = Logger<false>("[Syscall][Misc          ] ");
static Logger tty               = Logger<1>    ();

// RSX
static Logger rsx               = Logger<true> ("[RSX    ][Command       ] ");
static Logger vertex_shader     = Logger<false>("[Shader ][Vertex        ] ");
static Logger fragment_shader   = Logger<false>("[Shader ][Fragment      ] ");
static Logger shader_cache      = Logger<true> ("[Shader ][Cache         ] ");

// Loader
static Logger loader_elf        = Logger<1>    ("[Loader ][ELF           ] ");
static Logger loader_prx        = Logger<1>    ("[Loader ][PRX           ] ");

// Manager
static Logger manager_prx       = Logger<1>    ("[Manager][PRX           ] ");

// Other
static Logger memory            = Logger<true> ("[Other  ][Memory        ] ");
static Logger thread            = Logger<false>("[Other  ][Thread        ] ");
static Logger lle_module        = Logger<true> ("[LLE    ][Module        ] ");
static Logger filesystem        = Logger<true> ("[Other  ][Filesystem    ] ");
static Logger unimplemented     = Logger<true> ("[Other  ][Unimplemented ] ");

#undef true
#undef false

// We have 2 ways to create a log function
// MAKE_LOG_FUNCTION: Creates a log function which is toggleable but always killed for user-facing builds
// MAKE_LOG_FUNCTION_USER: Creates a log function which is toggleable, may be on for user builds as well
// We need this because sadly due to the loggers taking variadic arguments, compilers will not properly
// Kill them fully even when they're disabled. The only way they will is if the function with varargs is totally empty

#define MAKE_LOG_FUNCTION_USER(functionName, logger)                    \
	template <typename... Args>                                         \
	void functionName(const char* fmt, Args&&... args) {                \
		Log::logger.log(fmt, args...);                             \
	}                                                                   \
    template <typename... Args>                                         \
    void functionName##NoPrefix(const char* fmt, Args&&... args) {      \
        Log::logger.logNoPrefix(fmt, args...);                     \
    }

#ifdef CHONKYSTATION3_USER_BUILD
#define MAKE_LOG_FUNCTION(functionName, logger)             \
	template <typename... Args>                             \
	void functionName(const char* fmt, Args&&... args) {}   \
    template <typename... Args>                             \
	void functionName##NoPrefix(const char* fmt, Args&&... args) {}
#else
#define MAKE_LOG_FUNCTION(functionName, logger) MAKE_LOG_FUNCTION_USER(functionName, logger)
#endif
}