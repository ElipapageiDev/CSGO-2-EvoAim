#pragma once
#include <cstdint>

namespace Elipapagei
{
    static constexpr int dwEntityList = 0x1A157C8;
    static constexpr int dwLocalPlayerPawn = 0x1869D88;
    static constexpr int dwViewAngles = 0x1A8A810;
    static constexpr int dwViewMatrix = 0x1A80870;
    static constexpr int m_entitySpottedState = 0x23D0; // EntitySpottedState_t
    static constexpr int m_vecViewOffset = 0xCB0; // CNetworkViewOffsetVector
    static constexpr int m_iTeamNum = 0x3E3; // uint8
    static constexpr int m_iHealth = 0x344; // int32
    static constexpr int m_vOldOrigin = 0x1324; // Vector
    static constexpr int m_hPlayerPawn = 0x80C; // CHandle<C_CSPlayerPawn>
    static constexpr int m_hObserverTarget = 0x44; // CHandle<C_BaseEntity>
    static constexpr int m_bSpotted = 0x8; // bool
    static constexpr int m_bSpottedByMask = 0xC; // uint32[2]
}


