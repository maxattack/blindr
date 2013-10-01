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
#include "VectorMath.h"

// VECTORIZED PARAMETRIC CURVES
// These compute curves based on linear multiplication by a "cubic parameteric vector":
// U = < u^3, u^2, u, 1 >,

#define XY_ROTATION_MATRIX (mat(0, -1, 0, 0, 1, 0, 0, 0))

inline mat4 derivativeMatrix(mat4 m) {
  return mat(
    0, 0, 0, 0,
    3*m.m00, 3*m.m01, 3*m.m02, 3*m.m03,
    2*m.m10, 2*m.m11, 2*m.m12, 2*m.m13,
    m.m20, m.m21, m.m22, m.m23
  );
}

inline mat4 perpendicularMatrix(mat4 m) {
  // XY_ROTATION_MATRIX * derivativeMatrix(m)
  return mat(
    0, 0, 0, 0,
    3*m.m01, -3*m.m00, 3*m.m02, 3*m.m03,
    2*m.m11, -2*m.m10, 2*m.m12, 2*m.m13,
    m.m21, -m.m20, m.m22, m.m23
  );
}

inline mat4 linearMatrix(vec4 p0, vec4 p1) {
  return mat(p0, p1, vec(0,0,0,0), vec(0,0,0,0)) * mat(
    0, 0, 0, 0,
    0, 0, 0, 0,
    -1, 1, 0, 0,
    1, 0, 0, 0
  );
}

inline mat4 linearDerivMatrix(vec4 p0, vec4 p1) {
  return mat(p0, p1, vec(0,0,0,0), vec(0,0,0,0)) * mat(
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    -1, 1, 0, 0
  );  
}

inline mat4 hermiteMatrix(vec4 p0, vec4 p1, vec4 t0, vec4 t1) {
	return mat(p0, p1, t0, t1) * mat(
    2, -2, 1, 1, 
    -3, 3, -2, -1, 
    0, 0, 1, 0, 
    1, 0, 0, 0
  );
}

inline mat4 hermiteDerivMatrix(vec4 p0, vec4 p1, vec4 t0, vec4 t1) {
  return mat(p0, p1, t0, t1) * mat(
    0, 0, 0, 0, 
    6, -6, 3, 3, 
    -6, 6, -4, -2, 
    0, 0, 1, 0
  );
}

inline mat4 bezierMatrix(vec4 p0, vec4 p1, vec4 p2, vec4 p3) {
	return mat(p0, p1, p2, p3) * mat(
    -1, 3, -3, 1, 
    3, -6, 3, 0, 
    -3, 3, 0, 0, 
    1, 0, 0, 0
  );
}

inline mat4 bezierDerivMatrix(vec4 p0, vec4 p1, vec4 p2, vec4 p3) {
  return mat(p0, p1, p2, p3) * mat(
    0, 0, 0, 0, 
    -3, 9, -9, 3, 
    6, -12, 6, 0, 
    -3, 3, 0, 0
  );
}

inline mat4 quadraticBezierMatrix(vec4 p0, vec4 p1, vec4 p2) {
  return mat(vec(0,0,0,0), p0, p1, p2) * mat(
    0, 0, 0, 0,
    0, 1, -2, 1,
    0, -2, 2, 0,
    0, 1, 0, 0
  );
}

inline mat4 quadraticBezierDerivMatrix(vec4 p0, vec4 p1, vec4 p2) {
  return mat(vec(0,0,0,0), p0, p1, p2) * mat(
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 2, -4, 2,
    0, -2, 2, 0
  );
}

