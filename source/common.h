#pragma once

// common types
typedef unsigned char u8;
typedef signed char i8;
typedef unsigned short u16;
typedef signed short i16;
typedef unsigned int u32;
typedef signed int i32;
typedef unsigned long long  u64;
typedef long long           i64;

// common includes
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <DMFastDelegate.h>
using namespace std;

// useful functions
inline int max(int x, int y) { return x > y ? x : y; }
inline int min(int x, int y) { return x < y ? x : y; }
inline int sign(int x) { return (x == 0) ? 0 : (x < 0 ? -1 : 1); }
inline bool StrEqual(const char* a, const char* b) { return SDL_strcasecmp(a, b) == 0; }
inline bool StrEqual(string& s1, const char* s2) { return s1.empty() ? (s2[0] == 0) : (SDL_strcasecmp(s1.c_str(), s2) == 0); }
inline bool StrEqual(string& s1, string& s2) { return s1.empty() ? s2.empty() : (SDL_strcasecmp(s1.c_str(), s2.c_str()) == 0); }
inline bool HasExtension(const char *s, const char* ext) { const char *strExt = SDL_strrchr(s, '.'); return (ext) ? StrEqual(strExt, ext) : false; }

#include "graphicChunk.h"
#include "sourceFile.h"
#include "application.h"

extern class Application* gApp;		// manages general application highlevel control

#define TIMEDELTA (1.0f/60.0f)

