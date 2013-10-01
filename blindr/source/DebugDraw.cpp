//
//  DebugDraw.cpp
//  blindr
//
//  Created by Max Kaufmann on 9/30/13.
//  Copyright (c) 2013 Little Polygon. All rights reserved.
//

#include "DebugDraw.h"
#include "VectorGraphics.h"
#include "Blindr.h"

inline vec2 toPixels(b2Vec2 pos) { return PixelsPerMeter * vec(pos); }

void DebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	for(int i=0; i<vertexCount; ++i) {
		DrawSegment(vertices[i], vertices[(i+1)%vertexCount], color);
	}
}

void DebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	
	VectorGraphics::setFillColor(rgba(color.r, color.g, color.b, 0.75f));
	vec2 pivot = toPixels(vertices[vertexCount-1]);
	{
	Graphics::ScopedTransform pushMatrix( translationMatrix(pivot) );
	for(int i=0; i<vertexCount; ++i) {
		VectorGraphics::drawLineFill(toPixels(vertices[i])-pivot, toPixels(vertices[(i+1)%vertexCount])-pivot);
	}
	}
	for(int i=0; i<vertexCount; ++i) {
		DrawSegment(vertices[i], vertices[(i+1)%vertexCount], color);
	}
}

void DebugDraw::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
	VectorGraphics::setArcColor(rgb(color.r, color.g, color.b));
	VectorGraphics::setArcRadius(PixelsPerMeter * radius);
	VectorGraphics::setArcStroke(1);
	VectorGraphics::drawArc(toPixels(center));
}

void DebugDraw::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{
	VectorGraphics::setCircleColor(rgba(color.r, color.g, color.b, 0.75f));
	VectorGraphics::setCircleRadius(PixelsPerMeter * radius);
	VectorGraphics::drawCircle(toPixels(center));
	
	DrawSegment(center, center+radius*axis, color);
}


void DebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
	VectorGraphics::setUniformStroke(0.5f);
	VectorGraphics::setStrokeColor(rgb(color.r, color.g, color.b));
	VectorGraphics::drawLineStroke(toPixels(p1), toPixels(p2));
}

void DebugDraw::DrawTransform(const b2Transform& xf)
{

	b2Vec2 p1 = xf.p, p2;
	const float32 k_axisScale = 1.0f;

	p2 = p1 + k_axisScale * xf.q.GetXAxis();
	DrawSegment(p1,p2,b2Color(1,0,0));
	
	p2 = p1 + k_axisScale * xf.q.GetYAxis();
	DrawSegment(p1,p2,b2Color(0,1,0));
}

void DebugDraw::DrawPoint(const b2Vec2& p, float32 size, const b2Color& color)
{
	VectorGraphics::setCircleColor(rgb(color.r, color.g, color.b));
	VectorGraphics::setCircleRadius(PixelsPerMeter * size);
	VectorGraphics::drawCircle(toPixels(p));
}

void DebugDraw::DrawString(int x, int y, const char *string, ...)
{
	/* Unsupported as yet. Could replace with bitmap font renderer at a later date */
}

void DebugDraw::DrawAABB(b2AABB* aabb, const b2Color& c)
{
	b2Vec2 min = aabb->lowerBound;
	b2Vec2 max = aabb->upperBound;
	DrawSegment(min, b2Vec2(min.x, max.y), c);
	DrawSegment(min, b2Vec2(max.x, min.y), c);
	DrawSegment(max, b2Vec2(min.x, max.y), c);
	DrawSegment(max, b2Vec2(max.x, min.y), c);
}