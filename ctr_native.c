#define _CRT_SECURE_NO_WARNINGS
#define REBUILD_PC
#define CTR_NATIVE
#define USE_EXTENDED_PRIM_POINTERS 0
#define SDL_MAIN_HANDLED

#if __GNUC__
#include <SDL2/SDL.h>
#define _EnterCriticalSection(x)
#define EnterCriticalSection(x)
#define ExitCriticalSection()
#endif

#include "psx/types.h"
#include "psx/libetc.h"
#include "psx/libgte.h"
#include "psx/libgpu.h"
#include "psx/libspu.h"
#include "psx/libcd.h"
#include "psx/libapi.h"
#include "psx/strings.h"
#include "psx/inline_c.h"
#include "PsyX/PsyX_public.h"
#include "PsyX/PsyX_globals.h"
#include "PsyX/PsyX_render.h"

#ifdef _WIN32
#define CLOCKS_PER_SEC_FIX CLOCKS_PER_SEC
#else
#define CLOCKS_PER_SEC_FIX ((clock_t)1000000)
#endif

#include <time.h>
static clock_t startClock;
#define ResetRCnt(x) startClock = clock();
#define GetRCnt(x)   ((clock() - startClock) * 15720) / CLOCKS_PER_SEC_FIX

#define BUILD        926
#define u_char       unsigned char
#define u_short      unsigned short
#define u_int        unsigned int
#define u_long       unsigned int

#ifndef __GNUC__
#define _Static_assert(x)
#endif
#define __attribute__(x)

#define RECT RECT16
typedef enum
{
	PAD_ID_MOUSE = 0x1,
	PAD_ID_NEGCON = 0x2,
	PAD_ID_IRQ10_GUN = 0x3,
	PAD_ID_DIGITAL = 0x4,
	PAD_ID_ANALOG_STICK = 0x5,
	PAD_ID_GUNCON = 0x6,
	PAD_ID_ANALOG = 0x7,
	PAD_ID_MULTITAP = 0x8,
	PAD_ID_JOGCON = 0xe,
	PAD_ID_CONFIG_MODE = 0xf,
	PAD_ID_NONE = 0xf
} PadTypeID;

#include "ctr_native.h"
#include "platform.h"

#include "game_includes.h"

#include "game/zGlobal_DATA.c"
#include "game/zGlobal_SDATA.c"

static int frameGap = 2000;
static int frameCount = 0;
static int oldTicks = 0;

static void CalcFPS(void)
{
	if (frameCount++ != frameGap)
		return;

	frameCount = 0;
	int newTicks = SDL_GetTicks();
	int delta = newTicks - oldTicks;
	oldTicks = newTicks;

	printf("FPS: %d\n", (1000 * frameGap) / delta);
}

void PsyXKeyboardHandler(int key, char down)
{
	if (down == 0)
		key = 0;

	void SubmitName_UseKeyboard(int key);
	SubmitName_UseKeyboard(key);
}

#ifndef CC
#if __GNUC__
#if _WIN32
#ifndef __clang__
#define CC "MINGW-GCC"
#else
#define CC "MINGW-CLANG"
#endif
#else
#ifndef __clang__
#define CC "GCC"
#else
#define CC "CLANG"
#endif
#endif
#elif defined(_MSC_VER)
#define CC "MSVC"
#else
#define CC "Unknown"
#endif
#endif

int main(int argc, char *argv[])
{
	printf("[CTR Native] Starting...\n");
	fflush(stdout);

	char *sdlBasePath = SDL_GetBasePath();
	printf("[CTR Native] SDL base path: %s\n", sdlBasePath ? sdlBasePath : "(null)");
	fflush(stdout);

	char baseDir[1024];

	if (sdlBasePath)
	{
		strncpy(baseDir, sdlBasePath, sizeof(baseDir));
		SDL_free(sdlBasePath);
		char *sep = strrchr(baseDir, '\\');
		if (!sep)
			sep = strrchr(baseDir, '/');
		if (sep)
			*sep = '\0';
		sep = strrchr(baseDir, '\\');
		if (!sep)
			sep = strrchr(baseDir, '/');
		if (sep)
			*sep = '\0';
	}
	else
	{
		getcwd(baseDir, sizeof(baseDir));
	}

	printf("[CTR Native] Built with: " CC "\n");
	printf("[CTR Native] Base: %s\n", baseDir);
	fflush(stdout);

	chdir(baseDir);

#ifdef USE_16BY9
	printf("[CTR Native] Widescreen\n");
	Platform_Init("Crash Team Racing", 1280, 720);
#else
	printf("[CTR Native] 4:3\n");
	Platform_Init("Crash Team Racing", 800, 600);
#endif

	Platform_InitFilesystem("assets/ctr-u.bin");

	PsyX_SetSwapInterval(2);
	PsyX_EnableSwapInterval(1);

	g_cfg_controllerToSlotMapping[0] = 0;

	g_dbg_gameDebugKeys = PsyXKeyboardHandler;
	memset(&g_cfg_keyboardMapping, 0, sizeof(g_cfg_keyboardMapping));

	int result = DECOMP_main();

	Platform_Shutdown();
	return result;
}

void Platform_Init(const char *title, int width, int height)
{
	PsyX_Initialise(title, width, height, 0);
}

void Platform_Shutdown(void)
{
}

void Platform_BeginFrame(void)
{
	PsyX_BeginScene();
}

void Platform_EndFrame(void)
{
	PsyX_EndScene();
	CalcFPS();
}

int Platform_PollInput(void)
{
	return 1;
}

void Platform_InitFilesystem(const char *disc_image)
{
	PsyX_CDFS_Init(disc_image, 0, 0);
}

// NOTE(aalhendi): stub to make compile work for now
int NikoGetEnterKey(void)
{
	return 0;
}
void NikoCalcFPS(void)
{
	static int frameCount = 0;
	static Uint32 lastTime = 0;

	frameCount++;
	Uint32 now = SDL_GetTicks();

	if (now - lastTime >= 1000)
	{
		fprintf(stderr, "[FPS] %d fps, gGT->timer=%d, vsyncTillFlip=%d\n", frameCount * 1000 / (now - lastTime), (int)sdata->gGT->timer, sdata->vsyncTillFlip);
		frameCount = 0;
		lastTime = now;
	}
}
