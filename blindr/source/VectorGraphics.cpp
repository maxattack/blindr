#include "VectorGraphics.h"

#define CUBIC_RESOLUTION 256
#define CIRCLE_RESOLUTION 128

using namespace Graphics;

static GLuint hCubicVBO;
static GLuint hCircleVBO;
static GLuint hArcVBO;

//------------------------------------------------------------------------------
// TYPES
//------------------------------------------------------------------------------

struct CubicVertex { float x, y, z, w, s; };

class CubicRenderer : public CubicMaterial, public CubicShader {
public:
	void enable() {
		glBindBuffer(GL_ARRAY_BUFFER, hCubicVBO);
		// skip the first "zero" vertex
		setParameterPointer((float*)(sizeof(CubicVertex)), sizeof(CubicVertex));
		setSidePointer((float*)(sizeof(CubicVertex)+16), sizeof(CubicVertex));
	}
	
	void disable() {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
};

class CubicFillRenderer : public CubicFillMaterial, public CubicFillShader {
public:
	void enable() {
		glBindBuffer(GL_ARRAY_BUFFER, hCubicVBO);
		setParameterPointer(0, 2*sizeof(CubicVertex));
	}
	
	void disable() {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
};

class CircleRenderer : public CircleMaterial, public CircleShader {
public:
	void enable() {
		glBindBuffer(GL_ARRAY_BUFFER, hCircleVBO);
		setUnitPointer(0);
	}
	
	void disable() {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
};

class SimpleRenderer : public SimpleMaterial, public SimpleShader {

};

class ArcRenderer : public ArcMaterial, public ArcShader {
public:	
	void enable() {
		glBindBuffer(GL_ARRAY_BUFFER, hArcVBO);
		setUnitAndNormalPointer(0);
	}
	
	void disable() {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
};

//------------------------------------------------------------------------------
// GLOBALS
//------------------------------------------------------------------------------

static CircleRenderer circleRenderer;
static CubicRenderer cubicRenderer;
static CubicFillRenderer cubicFillRenderer;
static ArcRenderer arcRenderer;
static SimpleRenderer simpleRenderer;

//------------------------------------------------------------------------------
// INITIALIZATION
//------------------------------------------------------------------------------

void VectorGraphics::init() {
	
	{ // plot cubic vertices
		CubicVertex verts[CUBIC_RESOLUTION+1];
		//verts[0] = { 0, 0, 0, 0, 0 };
		verts[0].x = 0;
		verts[0].y = 0;
		verts[0].z = 0;
		verts[0].w = 0;
		verts[0].s = 0;
		for(int i=0; i<CUBIC_RESOLUTION/2; ++i) {
			float u = i/float(CUBIC_RESOLUTION/2-1);
			verts[1+2*i].x = u*u*u;
			verts[1+2*i].y = u*u;
			verts[1+2*i].z = u;
			verts[1+2*i].w = 1;
			verts[1+2*i].s = -0.5f;
			verts[1+(2*i)+1].x = u*u*u;
			verts[1+(2*i)+1].y = u*u;
			verts[1+(2*i)+1].z = u;
			verts[1+(2*i)+1].w = 1;
			verts[1+(2*i)+1].s = 0.5f;
		}
		glGenBuffers(1, &hCubicVBO);
		glBindBuffer(GL_ARRAY_BUFFER, hCubicVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(CubicVertex)*(CUBIC_RESOLUTION+1), verts, GL_STATIC_DRAW);
	}
	
	{ // plot unit circle
		vec2 verts[CIRCLE_RESOLUTION];
		vec2 unit = vec(1, 0);
		vec2 rot = polar(1, M_TAU/CIRCLE_RESOLUTION);
		for(int i=0; i<CIRCLE_RESOLUTION; ++i) {
			verts[i] = unit;
			unit = cmul(unit, rot);
		}
		glGenBuffers(1, &hCircleVBO);
		glBindBuffer(GL_ARRAY_BUFFER, hCircleVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec2)*CIRCLE_RESOLUTION, verts, GL_STATIC_DRAW);
	}
	
	{ // plot arc circle
		vec4 verts[2*CIRCLE_RESOLUTION];
		vec2 unit = vec(1, 0);
		vec2 rot = polar(1, M_TAU/(CIRCLE_RESOLUTION-1));
		for(int i=0; i<CIRCLE_RESOLUTION; ++i) {
			verts[2*i].x = unit.x;
			verts[2*i].y = unit.y;
			verts[2*i].z = 0.5f * unit.x;
			verts[2*i].w = 0.5f * unit.y;
			verts[2*i+1].x = unit.x;
			verts[2*i+1].y = unit.y;
			verts[2*i+1].z = -0.5f * unit.x;
			verts[2*i+1].w = -0.5f * unit.y;
			unit = cmul(unit, rot);
		}
		glGenBuffers(1, &hArcVBO);
		glBindBuffer(GL_ARRAY_BUFFER, hArcVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec4)*CIRCLE_RESOLUTION*2, verts, GL_STATIC_DRAW);
		
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	cubicRenderer.initializeMaterial(&cubicRenderer);
	circleRenderer.initializeMaterial(&circleRenderer);
	arcRenderer.initializeMaterial(&arcRenderer);
	cubicFillRenderer.initializeMaterial(&cubicFillRenderer);
	simpleRenderer.initializeMaterial(&simpleRenderer);
}

void VectorGraphics::release() {
	// todo
}

//------------------------------------------------------------------------------
// STROKE METHODS
//------------------------------------------------------------------------------

void VectorGraphics::setStrokeColor(Color c) {
	setMaterial(&cubicRenderer);
	cubicRenderer.setColor(c.red(), c.green(), c.blue(), c.alpha());
}

void VectorGraphics::setStrokeVector(vec4 v) {
	setMaterial(&cubicRenderer);
	cubicRenderer.setStrokeVector(v.x, v.y, v.z, v.w);
}

void VectorGraphics::setUniformStroke(float u) {
	setStrokeVector(vec(0, 0, 0, u));
}

void VectorGraphics::setTaperingStroke(float u, float v) {
	setStrokeVector(vec(0, 0, v-u, u));
}

void VectorGraphics::setEccentricStroke(float t0, float e, float t1) {
	setStrokeVector(vec(0, -e-e-e-e, e+e+e+e+t1-t0, t0));
}

void VectorGraphics::setQuadraticBezierStroke(float t0, float t1, float t2) {
	setStrokeVector(vec(0, t0-t1-t1+t2, -t0-t0+t1+t1, t0));
}

void VectorGraphics::drawStroke(const mat4 &curveMatrix, const mat4 &strokeMatrix) {
	setMaterial(&cubicRenderer);
	cubicRenderer.setPositionMatrix(curveMatrix.m);
	cubicRenderer.setStrokeMatrix(strokeMatrix.m);
	cubicRenderer.setMvp(matrix().m);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, CUBIC_RESOLUTION);
}

void VectorGraphics::drawStroke(const mat4 &curveMatrix) {
	drawStroke(curveMatrix, perpendicularMatrix(curveMatrix));
}

void VectorGraphics::drawLineStroke(vec2 p0, vec2 p1) {
	// todo: optimize by only plotting 4 vertices?
	drawStroke(
		linearMatrix(vec(p0, 0, 1), vec(p1, 0, 1)), 
		XY_ROTATION_MATRIX * linearDerivMatrix(vec(p0, 0, 1), vec(p1, 0, 1))
	);
}

void VectorGraphics::drawQuadraticBezierStroke(vec2 p0, vec2 p1, vec2 p2) {
	drawStroke(
		quadraticBezierMatrix(vec(p0, 0, 1), vec(p1, 0, 1), vec(p2, 0, 1)),
		XY_ROTATION_MATRIX * quadraticBezierDerivMatrix(vec(p0, 0, 1), vec(p1, 0, 1), vec(p2, 0, 1))
 	);
}

void VectorGraphics::drawCubicBezierStroke(vec2 p0, vec2 p1, vec2 p2, vec2 p3) {
	drawStroke(
		bezierMatrix(vec(p0, 0, 1), vec(p1, 0, 1), vec(p2, 0, 1), vec(p3, 0, 1)),
		XY_ROTATION_MATRIX * bezierDerivMatrix(vec(p0, 0, 1), vec(p1, 0, 1), vec(p2, 0, 1), vec(p3, 0, 1))
	);
}

void VectorGraphics::drawHermiteStroke(vec2 p0, vec2 t0, vec2 p1, vec2 t1) {
	drawStroke(
		hermiteMatrix(vec(p0, 0, 1), vec(p1, 0, 1), vec(t0, 0, 0), vec(t1, 0, 0)),
		XY_ROTATION_MATRIX * hermiteDerivMatrix(vec(p0, 0, 0), vec(p1, 0, 0), vec(t0, 0, 0), vec(t1, 0, 0))
	);
}

//------------------------------------------------------------------------------
// FILL METHODS
//------------------------------------------------------------------------------

void VectorGraphics::setFillColor(Color c) {
	setMaterial(&cubicFillRenderer);
	cubicFillRenderer.setColor(c.red(), c.green(), c.blue(), c.alpha());
}

void VectorGraphics::drawFill(const mat4 &curveMatrix) {
	setMaterial(&cubicFillRenderer);
	cubicFillRenderer.setPositionMatrix(curveMatrix.m);
	cubicFillRenderer.setMvp(matrix().m);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 1+(CUBIC_RESOLUTION>>1));
}

void VectorGraphics::drawLineFill(vec2 p0, vec2 p1) {
	drawFill(
		linearMatrix(vec(p0, 0, 1), vec(p1, 0, 1))
	);
	
}

void VectorGraphics::drawQuadraticBezierFill(vec2 p0, vec2 p1, vec2 p2) {
	drawFill(
		quadraticBezierMatrix(vec(p0, 0, 1), vec(p1, 0, 1), vec(p2, 0, 1))
	);
	
}

void VectorGraphics::drawCubicBezierFill(vec2 p0, vec2 p1, vec2 p2, vec2 p3) {
	drawFill(
		 bezierMatrix(vec(p0, 0, 1), vec(p1, 0, 1), vec(p2, 0, 1), vec(p3, 0, 1))
	 );
	
}

void VectorGraphics::drawHermiteFill(vec2 p0, vec2 t0, vec2 p1, vec2 t1) {
	drawFill(
		hermiteMatrix(vec(p0, 0, 1), vec(p1, 0, 1), vec(t0, 0, 0), vec(t1, 0, 0))
	);
}

//------------------------------------------------------------------------------
// CIRCLE METHODS
//------------------------------------------------------------------------------

void VectorGraphics::setCircleColor(Color c) {
	setMaterial(&circleRenderer);
	circleRenderer.setColor(c.red(), c.green(), c.blue(), c.alpha());
}

void VectorGraphics::setCircleRadius(float r) {
	setMaterial(&circleRenderer);
	circleRenderer.setRadius(r);
}

void VectorGraphics::drawCircle(vec2 p, float depth) {
	setMaterial(&circleRenderer);
	circleRenderer.setMvp((matrix() * translationMatrix(p, depth)).m);
	glDrawArrays(GL_TRIANGLE_FAN, 0, CIRCLE_RESOLUTION);
}

#define SIMPLE_CAPACITY 256
static vec2 simpleBuffer[SIMPLE_CAPACITY];

void VectorGraphics::drawSemicircle(vec2 p, Color c, float r, float startAngle, float endAngle, int resolution) {
	ASSERT(resolution + 2 <= SIMPLE_CAPACITY);
	setMaterial(&simpleRenderer);
	simpleRenderer.setMvp((matrix() * translationMatrix(p, 0.0f)).m);
	simpleRenderer.setColor(c.red(), c.green(), c.blue(), c.alpha());
	
	vec2 *verts = simpleBuffer;
	verts[0] = vec(0,0);
	vec2 unit = polar(r, startAngle);
	vec2 rot = polar(1, (endAngle - startAngle)/resolution);
	for(int i=0; i<=resolution; ++i) {
		verts[i+1] = unit;
		unit = cmul(unit, rot);
	}
	
	simpleRenderer.setVertexPointer((float*)verts);
	glDrawArrays(GL_TRIANGLE_FAN, 0, resolution+2);
	simpleRenderer.setVertexPointer(0);
}

//------------------------------------------------------------------------------
// ARC METHODS
//------------------------------------------------------------------------------

void VectorGraphics::setArcColor(Color c) {
	setMaterial(&arcRenderer);
	arcRenderer.setColor(c.red(), c.green(), c.blue(), c.alpha());
}

void VectorGraphics::setArcRadius(float r) {
	setMaterial(&arcRenderer);
	arcRenderer.setRadius(r);
}

void VectorGraphics::setArcStroke(float s) {
	setMaterial(&arcRenderer);
	arcRenderer.setStroke(s);
}


void VectorGraphics::drawArc(vec2 p, float depth) {
	setMaterial(&arcRenderer);
	arcRenderer.setMvp((matrix() * translationMatrix(p, depth)).m);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 2*CIRCLE_RESOLUTION);
}
