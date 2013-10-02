#pragma once
#include "ParametricMath.h"
#include "Graphics.h"
#include "VectorAssets.h"

namespace VectorGraphics {

void init();
void release();

void setStrokeColor(Color c);
void setStrokeVector(vec4 v);
void setUniformStroke(float u);
void setTaperingStroke(float u, float v);
void setEccentricStroke(float u, float e, float v);
void setQuadraticBezierStroke(float t0, float t1, float t2);

void drawStroke(const mat4 &curveMatrix, const mat4 &strokeMatrix);
void drawStroke(const mat4 &curveMatrix);
void drawLineStroke(vec2 p0, vec2 p1);
void drawQuadraticBezierStroke(vec2 p0, vec2 p1, vec2 p2);
void drawCubicBezierStroke(vec2 p0, vec2 p1, vec2 p2, vec2 p3);
void drawHermiteStroke(vec2 p0, vec2 t0, vec2 p1, vec2 t1);

void setFillColor(Color c);
void drawFill(const mat4 &curveMatrix);
void drawLineFill(vec2 p0, vec2 p1);
void drawQuadraticBezierFill(vec2 p0, vec2 p1, vec2 p2);
void drawCubicBezierFill(vec2 p0, vec2 p1, vec2 p2, vec2 p3);
void drawHermiteFill(vec2 p0, vec2 t0, vec2 p1, vec2 t1);

void setCircleColor(Color c);
void setCircleRadius(float r);
void drawCircle(vec2 p, float depth=0);

void drawSemicircle(vec2 p, Color c, float r, float startAngle, float endAngle, int resolution);
void fillScreen(Color c);

void setArcColor(Color c);
void setArcRadius(float r);
void setArcStroke(float s);
void drawArc(vec2 p, float depth=0);



}