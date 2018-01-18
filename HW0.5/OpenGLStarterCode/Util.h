#ifndef UTIL_INCLUDED
#define UTIL_INCLUDED

#include "glad/glad.h"  //Include order can matter here
#ifdef __APPLE__
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#else
#include <SDL.h>
#include <SDL_opengl.h>
#endif

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>

namespace util
{
	//test if a key is down
	bool isKeyDown(int scancode);

}

#endif