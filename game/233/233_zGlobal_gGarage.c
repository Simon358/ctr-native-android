#include <common.h>

// NOTE(aalhendi): These are native objects for retail overlay 233 data/BSS.
// PS1 address comments remain in include/ovr_233.h for ASM audit, but native
// code must not dereference those absolute PS1 addresses.
typeof(OVR_233) OVR_233 __attribute__((section(".data")));

struct OVR233_Garage gGarage = {
    .menuGarage =
        {
            .stringIndexTitle = 0xFFFF,
            .posX_curr = 0x100,
            .posY_curr = 0x6c,

            .unk1 = 0,

            .state = 0x823,
            .rows = 0,
            .funcPtr = DECOMP_CS_Garage_MenuProc,
            .drawStyle = 0,
        },

    .numFramesMax_GarageMove = 0x1d,

    // used???
    .padding1 = 0x14,

    .numFramesMax_Zoom = 0x14,

    .fovMin = 0x12c,
    .fovMax = 0x190,

    // only used for CS_Thread_UseOpcode,
    // should remove later
    .unusedArr_garageChars = {0, 1, 2, 3, 4, 5, 6, 7},

    // barLen is used, intended zero

    // unusedFrameCount (ignore)

    .unusedArr_lngIndex =
        {
            0x248,
            0x249,
            0x24a,
            0,
        },

    .barStat =
        {
            // balanced
            0x37,
            0x37,
            0x37,

            0x30,
            0x50,
            0x20,

            0x50,
            0x20,
            0xA,

            // turn
            0x1c,
            0x30,
            0x50,
        },

    .unusedArr_Colors =
        {
            0xff,
            0xff00,
            0xff00ff,
        },

    .barColors =
        {
            // blue
            0xc80000,

            // blue-green
            0xA8700,

            // green-yellow
            0xb428,

            // yellow
            0xb4b4,

            // orange
            0x64dc,

            // dark orange
            0x28dc,

            // red
            0xeb,
        },

    // rest initialize to zero
};

struct Ovr233_Credits_BSS creditsBSS;
