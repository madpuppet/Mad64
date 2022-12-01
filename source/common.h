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
#include <cmath>
#include <algorithm>
#include <DMFastDelegate.h>
using namespace std;

// useful functions
inline int max(int x, int y) { return x > y ? x : y; }
inline int min(int x, int y) { return x < y ? x : y; }
inline int sign(int x) { return (x == 0) ? 0 : (x < 0 ? -1 : 1); }
inline int lerp(int v1, int v2, float blend) { return (int)(v1 + (v2 - v1) * blend); }

inline float max(float x, float y) { return x > y ? x : y; }
inline float min(float x, float y) { return x < y ? x : y; }
inline float sign(float x) { return (x == 0) ? 0 : (x < 0 ? -1.0f : 1.0f); }
inline float lerp(float v1, float v2, float blend) { return (v1 + (v2 - v1) * blend); }

// does a range contain a value (NOTE: max range is exclusive so s1=0, s2=5 includes all values 0,1,2,3,4 but not 5
inline bool contains(int s1, int s2, int val) { return val >= s1 && val < s2; }
inline bool containsRange(int s1, int s2, int r1, int r2) { return r2 >= s1 && r1 < s2; }

inline bool StrEqual(const char* a, const char* b) { return SDL_strcasecmp(a, b) == 0; }
inline bool StrEqual(const string& s1, const char* s2) { return s1.empty() ? (s2[0] == 0) : (SDL_strcasecmp(s1.c_str(), s2) == 0); }
inline bool StrEqual(const string& s1, const string& s2) { return s1.empty() ? s2.empty() : (SDL_strcasecmp(s1.c_str(), s2.c_str()) == 0); }
inline bool StrEqual(const string& s1, const string& s2, size_t len) { return s1.empty() ? s2.empty() : (SDL_strncasecmp(s1.c_str(), s2.c_str(), len) == 0); }

inline bool HasExtension(const char *s, const char* ext) { const char *strExt = SDL_strrchr(s, '.'); return (strExt) ? StrEqual(strExt, ext) : false; }
extern string GetPath(const string& path);
extern void CopyToClipboard(vector<string>& buffer);
extern void CopyFromClipboard(vector<string>& buffer);
extern char KeySymToAscii(const SDL_Keysym &sym);

inline size_t StrFind(const string& haystack, const string& needle, size_t offset = 0)
{
	size_t needleSize = needle.size();
	if (haystack.size() < needleSize || needleSize == 0)
		return string::npos;
	size_t maxidx = haystack.size() - needle.size();
	for (size_t i = offset; i <= maxidx; i++)
	{
		if (SDL_strncasecmp(&haystack[i], &needle[0], needleSize)==0)
			return i;
	}
	return string::npos;
}
inline size_t StrFind(const char *haystack, const string& needle, size_t offset = 0)
{
	size_t needleSize = needle.size();
	size_t haystackSize = strlen(haystack);
	if (haystackSize < needleSize || needleSize == 0)
		return string::npos;
	size_t maxidx = haystackSize - needle.size();
	for (size_t i = offset; i <= maxidx; i++)
	{
		if (SDL_strncasecmp(&haystack[i], &needle[0], needleSize) == 0)
			return i;
	}
	return string::npos;
}

extern string FormatString(const char* pFormat, ...);
#define STR(...)  FormatString(__VA_ARGS__);

class Profile
{
public:
	Profile(string label) { m_label = label; m_start = SDL_GetPerformanceCounter(); }
	string Log()
	{
		u64 duration = SDL_GetPerformanceCounter() - m_start;
		float ms = (float)((double)duration * 1000.0 / (double)SDL_GetPerformanceFrequency());
		return FormatString("%s: %1.4fms", m_label.c_str(), ms);
	}
	double Time()
	{
		u64 duration = SDL_GetPerformanceCounter() - m_start;
		return ((double)duration * 1000.0 / (double)SDL_GetPerformanceFrequency());
	}
	string m_label;
	u64 m_start;
};

inline bool Contains(const SDL_Rect& rect, int x, int y)
{
	return (x >= rect.x && x < (rect.x + rect.w) && y >= rect.y && y < (rect.y + rect.h));
}

inline bool IsNear(int a, int b, int range)
{
	return (abs(a - b) <= range);
}

extern u32string UTF8toUNICODE(string text);
extern void LogStart();
extern void Log(const char* pFormat, ...);

typedef DMFastDelegate::FastDelegate1<u16, u8> ReadByteHook;
typedef DMFastDelegate::FastDelegate2<u16, u8> WriteByteHook;
typedef DMFastDelegate::FastDelegate3<u8, u16, u8> BreakpointHook;
typedef DMFastDelegate::FastDelegate0<> InterruptHook;
typedef DMFastDelegate::FastDelegate1<u8> SetByteHook;

enum BreakpointType
{
	BRK_Read     = 1,
	BRK_Write    = 2,
	BRK_Execute  = 4,

	BRK_ALL = BRK_Read|BRK_Write|BRK_Execute
};

#include "application.h"

extern class Application* gApp;		// manages general application highlevel control

#define TIMEDELTA gApp->GetTimeDelta()

