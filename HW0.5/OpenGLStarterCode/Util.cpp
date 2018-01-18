#include "Util.h"

bool util::isKeyDown(int scancode)
{
	return SDL_GetKeyboardState(NULL)[scancode];
}
