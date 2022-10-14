#pragma once

// common types
typedef unsigned char u8;
typedef signed char i8;
typedef unsigned short u16;
typedef signed short i16;
typedef unsigned int u32;
typedef signed int i32;

// common includes
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <DMFastDelegate.h>
using namespace std;

#include "graphicChunk.h"
#include "sourceFile.h"
#include "uimanager.h"
#include "application.h"

extern class Application* gApp;		// manages general application highlevel control


