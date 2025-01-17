#pragma once


#include "../offsets/offsets.h"
#include "../math/vector.h"
#include "../memory/memory.h"

namespace aimbot
{
    inline uintptr_t module_base;
    inline uintptr_t procID;

        extern bool enabled22;        
        extern float fovLimit;
        extern float speedLimit;
        extern bool visibilityCheck;

        void enable();
        void disable();
        bool isEnabled();
        void frame();

}
