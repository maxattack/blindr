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
#include "Macros.h"

// CONSTANTS

#define M_TAU             (M_PI+M_PI)
#define M_COLINEAR_SLOP   (0.0001f)

// UTILITY FUNCTIONS

inline float clamp(float u, float lo=0.f, float hi=1.f) { return u<lo?lo:u>hi?hi:u; }
inline float lerp(float u, float v, float t) { return u + t * (v - u); }

struct vec2 {
	float x;
	float y;
	
	float real() const         { return x; }
	float imag() const         { return y; }
	float norm() const         { return x*x + y*y; }
	float manhattan() const    { return fabs(x)+fabs(y); }
	float magnitude() const    { return sqrtf(norm()); }
	vec2 conjugate() const     { return {x,-y}; }
	float radians() const      { return atan2f(y,x); }
	vec2 reflection() const    { return {y,x}; }
	vec2 anticlockwise() const { return {-y, x}; }
	vec2 clockwise() const     { return {y, -x}; }
	vec2 normalized() const    { return (*this) / magnitude(); }

	vec2 operator+(vec2 q) const  { return {x+q.x, y+q.y}; }
	vec2 operator-(vec2 q) const  { return {x-q.x, y-q.y}; }
	vec2 operator*(vec2 q) const  { return {x*q.x, y*q.y}; }
	vec2 operator/(vec2 q) const  { return {x/q.x, y/q.y}; }

	vec2 operator-() const        { return {-x, -y}; }
	vec2 operator*(float k) const { return {k*x, k*y}; }
	vec2 operator/(float k) const { return {x/k, y/k}; }

	vec2 operator +=(vec2 u)  { x+=u.x; y+=u.y; return *this; }
	vec2 operator -=(vec2 u)  { x-=u.x; y-=u.y; return *this; }
	vec2 operator *=(vec2 u)  { x*=u.x; y*=u.y; return *this; }
	vec2 operator /=(vec2 u)  { x/=u.x; y/=u.y; return *this; }

	vec2 operator *=(float k) { x*=k; y*=k; return *this; }
	vec2 operator /=(float k) { x/=k; y/=k; return *this; }
};
	
inline vec2 vec(float ax, float ay) { return {ax, ay}; }
inline vec2 cmul(vec2 u, vec2 v) { return {u.x*v.x-u.y*v.y, u.x*v.y+u.y*v.x}; }
inline vec2 cdiv(vec2 u, vec2 v) {
	float normInv = 1.0f/v.norm();
	return {(u.x*v.x+u.y*v.y)*normInv, (v.x*u.y-u.x*v.y)*normInv};
}
inline vec2 polar(float radius, float radians) { return {radius*cosf(radians), radius*sinf(radians)}; }
inline float easeOut2(float u) { return 1.0 - (u=1.0-u)*u; }
inline float easeOut4(float u) { return 1.0 - (u=1.0-u)*u*u*u; }
inline float easeInOutBack(float t, float s=1.70158f) { return (s+1)*t*t*t - s*t*t; }
inline float easeTowards(float curr, float target, float easing, float dt) { return curr + powf(easing, dt) * (target - curr); }
inline int randInt(int x) { return rand() % x; }
inline double randomValue() { return rand() / double(RAND_MAX); }
inline double randomValue(double u, double v) { return u + randomValue() * (v - u); }
inline double expovariate(double avgDuration) { return -avgDuration * log(1.0 - randomValue(0.0000001, 0.999999)); }
	
inline vec2 operator*(float k, vec2 q) { return {k*q.x, k*q.y}; }
inline float dot(vec2 u, vec2 v) { return u.x*v.x + u.y*v.y; }
inline float cross(vec2 u, vec2 v) { return u.x * v.y - v.x* u.y; }
inline vec2 lerp(vec2 u, vec2 v, float t) { return u + t * (v - u); }
inline vec2 slerp(vec2 u, vec2 v, float t) {
	float theta = acosf(dot(u,v));
	float s = 1.f/sinf(theta);
	return (sinf((1-t)*theta)*s)*u + (sinf(t*theta)*s)*v;
}

inline float normalizeAngle(float radians) {
	radians = fmodf(radians, M_TAU);
	if (radians < 0) { radians += M_TAU; }
	return radians;
}
	
inline float radianDiff(float lhs, float rhs) {
	float result = normalizeAngle(lhs - rhs);
	if (result > M_PI) {
		return result - M_TAU;
	} else if (result < -M_PI) {
		return result + M_TAU;
	} else {
		return result;
	}
}
	
bool linearIntersection(vec2 u0, vec2 u1, vec2 v0, vec2 v1, float& u);
bool linearIntersection(vec2 u0, vec2 u1, vec2 v0, vec2 v1, float& u, float& v);

vec2 quadraticBezier(vec2 p0, vec2 p1, vec2 p2, float u);
vec2 quadraticBezierDeriv(vec2 p0, vec2 p1, vec2 p2, float u);
vec2 cubicBezier(vec2 p0, vec2 p1, vec2 p2, vec2 p3, float u);
vec2 cubicBezierDeriv(vec2 p0, vec2 p1, vec2 p2, vec2 p3, float u);
vec2 cubicHermite(vec2 p0, vec2 m0, vec2 p1, vec2 m1, float u);
vec2 cubicHermiteDeriv(vec2 p0, vec2 m0, vec2 p1, vec2 m1, float u);

struct AABB {
	float x, y, w, h;
	
	vec2 extentMin() const { return vec(x,y); }
	vec2 size() const { return vec(w,h); }
	vec2 extentMax() const { return extentMin() + size(); }
	
	bool contains(vec2 p) { return p.x > x && p.y > y && p.x < x+w && p.y < y+h; }
	AABB inflate(float pad) { return { x-pad, y-pad, w+pad+pad, h+pad+pad }; }
};

inline AABB aabb(float x, float y, float w, float h) { return { x, y, w, h }; }
inline AABB aabb(vec2 p, vec2 s) { return { p.x, p.y, s.x, s.y }; }
inline AABB unionOf(AABB u, AABB v) { return aabb(MIN(u.x, v.x), MIN(u.y, v.y), MAX(u.x+u.w, v.x+v.w), MAX(u.y+u.h, v.y+v.h)); }

struct ivec2 {
	int x, y;
	
	inline vec2 toFloat() { return vec(x, y); }
	inline ivec2 operator+(const ivec2& rhs) { return {x+rhs.x, y+rhs.y}; }
	inline ivec2 operator-(const ivec2& rhs) { return {x-rhs.x, y-rhs.y}; }
	inline ivec2 operator*(const ivec2& rhs) { return {x*rhs.x, y*rhs.y}; }
	inline ivec2 operator/(const ivec2& rhs) { return {x/rhs.x, y/rhs.y}; }
};

inline ivec2 ivec(int x, int y) { return {x, y}; }
inline ivec2 floor(vec2 v) { return { int(v.x), int(v.y) }; }


