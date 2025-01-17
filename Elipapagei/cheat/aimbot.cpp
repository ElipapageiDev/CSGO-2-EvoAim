#include "aimbot.h"


bool spotted = true;
int previousTotalHits = 0;

namespace aimbot {
    bool enabled22 = false;  
    float fovLimit = 30.0f;
    float speedLimit = 100.0f;
    bool visibilityCheck = true;  

   
    void enable() {
        enabled22 = true;
    }

   
    void disable() {
        enabled22 = false;
    }

   
    bool isEnabled() {
        return enabled22;
    }

   
    float distance(vec3 p1, vec3 p2)
    {
        return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2) + pow(p1.z - p2.z, 2));
    }

    
    float getFov(const vec3& viewAngle, const vec3& aimAngle)
    {
        vec3 delta;
        delta.x = aimAngle.x - viewAngle.x;
        delta.y = aimAngle.y - viewAngle.y;
        delta.z = aimAngle.z - viewAngle.z;

        float fov = sqrt(pow(delta.x, 2) + pow(delta.y, 2));
        return fov;
    }

    
    void frame()
    {
        if (!enabled22)  
            return;

        uintptr_t entityList = memory::Read<uintptr_t>(module_base + Elipapagei::dwEntityList);
        if (!entityList)
            return;

        uintptr_t localPlayerPawn = memory::Read<uintptr_t>(module_base + Elipapagei::dwLocalPlayerPawn);
        if (!localPlayerPawn)
            return;

        BYTE team = memory::Read<BYTE>(localPlayerPawn + Elipapagei::m_iTeamNum);
        vec3 localEyePos = memory::Read<vec3>(localPlayerPawn + Elipapagei::m_vOldOrigin) +
            memory::Read<vec3>(localPlayerPawn + Elipapagei::m_vecViewOffset);

        vec3 viewAngles = memory::Read<vec3>(module_base + Elipapagei::dwViewAngles);

        float closest_distance = -1;
        vec3 enemyPos;
        vec3 bestAngle;
        float bestFov = fovLimit;

        for (int i = 0; i < 32; i++)
        {
            uintptr_t listEntry = memory::Read<uintptr_t>(entityList + ((8 * (i & 0x7ff) >> 9) + 16));
            if (!listEntry) continue;

            uintptr_t entityController = memory::Read<uintptr_t>(listEntry + 120 * (i & 0x1ff));
            if (!entityController) continue;

            uintptr_t entityControllerPawn = memory::Read<uintptr_t>(entityController + Elipapagei::m_hPlayerPawn);
            if (!entityControllerPawn) continue;

            uintptr_t entityPawn = memory::Read<uintptr_t>(listEntry + 120 * (entityControllerPawn & 0x1ff));
            if (!entityPawn) continue;

            if (team == memory::Read<BYTE>(entityPawn + Elipapagei::m_iTeamNum))
                continue;

            if (memory::Read<std::uint32_t>(entityPawn + Elipapagei::m_iHealth) <= 0)
                continue;

            if (visibilityCheck)
            {
                bool entitySpotted = memory::Read<bool>(entityPawn + Elipapagei::m_entitySpottedState + Elipapagei::m_bSpotted);

                if (!entitySpotted)
                    continue;
            }

            vec3 entityEyePos = memory::Read<vec3>(entityPawn + Elipapagei::m_vOldOrigin) +
                memory::Read<vec3>(entityPawn + Elipapagei::m_vecViewOffset);

            float current_distance = distance(localEyePos, entityEyePos);
            vec3 aimAngle = (entityEyePos - localEyePos).RelativeAngle();

            float currentFov = getFov(viewAngles, aimAngle);

            if (currentFov < bestFov && (closest_distance < 0 || current_distance < closest_distance))
            {
                bestFov = currentFov;
                closest_distance = current_distance;
                enemyPos = entityEyePos;
                bestAngle = aimAngle;
            }
        }

        if (closest_distance >= 0 && bestFov <= fovLimit)
        {
            vec3 deltaAngle = bestAngle - viewAngles;

            float speedFactor = 101.0f - speedLimit;
            deltaAngle.x = deltaAngle.x / speedFactor;
            deltaAngle.y = deltaAngle.y / speedFactor;

            vec3 newAngle = viewAngles + deltaAngle;

            memory::Write<vec3>(module_base + Elipapagei::dwViewAngles, newAngle);
        }
    }
}
