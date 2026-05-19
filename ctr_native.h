#ifndef CTR_NATIVE_H
#define CTR_NATIVE_H

#include <SDL2/SDL.h>

struct ctr_state
{
	SDL_Window *window;
	int window_width;
	int window_height;
	int running;
};

#ifdef CTR_NATIVE
int NikoGetEnterKey(void);
void SubmitName_UseKeyboard(int key);
#endif

#endif
