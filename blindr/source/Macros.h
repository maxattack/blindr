// Little Polygon SDK
// Copyright (C) 2013 Max Kaufmann
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once
#define _USE_MATH_DEFINES
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <cstring>
#include <stdint.h>
#include <limits.h>

#define STATIC_ASSERT(_x)  ((void)sizeof(char[1 - 2*!(_x)]))

#ifndef arraysize
#define arraysize(a)   (sizeof(a) / sizeof((a)[0]))
#endif

#ifndef offsetof
#define offsetof(t,m)  ((uintptr_t)(uint8_t*)&(((t*)0)->m))
#endif

#ifndef MIN
#define MIN(a,b)   ((a) < (b) ? (a) : (b))
#define MAX(a,b)   ((a) > (b) ? (a) : (b))
#endif

#define CLZ(_expr)	        (__builtin_clz(_expr))

#ifdef DEBUG
#   define ASSERT(cond)     (assert(cond))
#	define CHECK(cond)      {int _result=(cond); ASSERT(_result);}
#   define LOG(_x)          printf _x
#   define LOG_MSG(_msg)    printf("%s:%d " _msg "\n", __FILE__, __LINE__)
#	define LOG_INT(_expr)	printf("%s:%d " #_expr " = %d\n", __FILE__, __LINE__, (_expr))
#	define LOG_FLOAT(_expr)	printf("%s:%d " #_expr " = %f\n", __FILE__, __LINE__, (_expr))
#	define LOG_VEC(_expr)	{ vec2 __u__ = (_expr); printf("%s:%d " #_expr " = <%f,%f>\n", __FILE__, __LINE__, __u__.x, __u__.y); }
#   define LOG_VEC4(_expr)  { vec4 __u__ = (_expr); printf("%s:%d " #_expr " = <%f,%f,%f,%f>\n", __FILE__, __LINE__, __u__.x, __u__.y, __u__.z, __u__.w); }
#else
#   define ASSERT(cond)
#   define CHECK(cond)      {if (!(cond)) { puts("FATAL: ##cond"); exit(-1); }}
#   define LOG(_x)
#   define LOG_MSG(_msg)
#	define LOG_INT(_expr)
#	define LOG_FLOAT(_expr)
#	define LOG_VEC(_expr)
#endif

#define SDL_CALLBACK extern "C"

#define COROUTINE_PARAM               int _line;
#define COROUTINE_RESET               _line=0;
#define COROUTINE_DISABLE             _line=-1;
#define COROUTINE_ACTIVE              (_line!=-1)
#define COROUTINE_BEGIN               switch(_line){case 0:;
#define COROUTINE_YIELD               {_line=__LINE__; return; case __LINE__:;}
#define COROUTINE_YIELD_RESULT(_x)    {_line=__LINE__;return (_x); case __LINE__:;}
#define COROUTINE_END                 _line=-1;default:;}

struct Coroutine {
	COROUTINE_PARAM;	
	void resetCoroutine() { _line = 0; }
};
