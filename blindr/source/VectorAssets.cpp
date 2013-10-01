#include "VectorAssets.h"

static const char sArcShaderVertSrc[] = 
	"uniform mediump float stroke;\n"
	"uniform mediump float radius;\n"
	"uniform mediump mat4 mvp;\n"
	"attribute highp vec4 unitAndNormal;\n"
	"void main ()\n"
	"{\n"
	"  highp vec4 tmpvar_1;\n"
	"  tmpvar_1.zw = vec2(0.0, 1.0);\n"
	"  tmpvar_1.xy = ((radius * unitAndNormal.xy) + (stroke * unitAndNormal.zw));\n"
	"  gl_Position = (mvp * tmpvar_1);\n"
	"}\n";

static const char sArcShaderFragSrc[] = 
	"uniform lowp vec4 color;\n"
	"void main ()\n"
	"{\n"
	"  gl_FragColor = color;\n"
	"}\n";

void ArcShader::initializeShader() {
	compile(sArcShaderVertSrc, sArcShaderFragSrc);
	mUnitAndNormal = glGetAttribLocation(progHandle(), "unitAndNormal");
	mMvp = glGetUniformLocation(progHandle(), "mvp");
	mColor = glGetUniformLocation(progHandle(), "color");
	mStroke = glGetUniformLocation(progHandle(), "stroke");
	mRadius = glGetUniformLocation(progHandle(), "radius");
	mAttribBegin = &mUnitAndNormal;
	mAttribEnd = &mUnitAndNormal+1;
}


static const char sCircleShaderVertSrc[] = 
	"uniform mediump float radius;\n"
	"uniform mediump mat4 mvp;\n"
	"attribute highp vec2 unit;\n"
	"void main ()\n"
	"{\n"
	"  highp vec4 tmpvar_1;\n"
	"  tmpvar_1.zw = vec2(0.0, 1.0);\n"
	"  tmpvar_1.xy = (radius * unit);\n"
	"  gl_Position = (mvp * tmpvar_1);\n"
	"}\n";

static const char sCircleShaderFragSrc[] = 
	"uniform lowp vec4 color;\n"
	"void main ()\n"
	"{\n"
	"  gl_FragColor = color;\n"
	"}\n";

void CircleShader::initializeShader() {
	compile(sCircleShaderVertSrc, sCircleShaderFragSrc);
	mUnit = glGetAttribLocation(progHandle(), "unit");
	mMvp = glGetUniformLocation(progHandle(), "mvp");
	mColor = glGetUniformLocation(progHandle(), "color");
	mRadius = glGetUniformLocation(progHandle(), "radius");
	mAttribBegin = &mUnit;
	mAttribEnd = &mUnit+1;
}


static const char sSimpleShaderVertSrc[] = 
	"uniform mediump mat4 mvp;\n"
	"attribute mediump vec2 vertex;\n"
	"void main ()\n"
	"{\n"
	"  mediump vec4 tmpvar_1;\n"
	"  tmpvar_1.zw = vec2(0.0, 1.0);\n"
	"  tmpvar_1.xy = vertex;\n"
	"  mediump vec4 tmpvar_2;\n"
	"  tmpvar_2 = (mvp * tmpvar_1);\n"
	"  gl_Position = tmpvar_2;\n"
	"}\n";

static const char sSimpleShaderFragSrc[] = 
	"uniform lowp vec4 color;\n"
	"void main ()\n"
	"{\n"
	"  gl_FragColor = color;\n"
	"}\n";

void SimpleShader::initializeShader() {
	compile(sSimpleShaderVertSrc, sSimpleShaderFragSrc);
	mVertex = glGetAttribLocation(progHandle(), "vertex");
	mMvp = glGetUniformLocation(progHandle(), "mvp");
	mColor = glGetUniformLocation(progHandle(), "color");
	mAttribBegin = &mVertex;
	mAttribEnd = &mVertex+1;
}


static const char sCubicShaderVertSrc[] = 
	"uniform mediump vec4 strokeVector;\n"
	"uniform mediump mat4 strokeMatrix;\n"
	"uniform mediump mat4 positionMatrix;\n"
	"uniform mediump mat4 mvp;\n"
	"attribute mediump float side;\n"
	"attribute mediump vec4 parameter;\n"
	"void main ()\n"
	"{\n"
	"  mediump vec4 tmpvar_1;\n"
	"  tmpvar_1.w = 1.0;\n"
	"  tmpvar_1.xyz = ((positionMatrix * parameter) + ((dot (strokeVector, parameter) * side) * normalize((strokeMatrix * parameter)))).xyz;\n"
	"  mediump vec4 tmpvar_2;\n"
	"  tmpvar_2 = (mvp * tmpvar_1);\n"
	"  gl_Position = tmpvar_2;\n"
	"}\n";

static const char sCubicShaderFragSrc[] = 
	"uniform lowp vec4 color;\n"
	"void main ()\n"
	"{\n"
	"  gl_FragColor = color;\n"
	"}\n";

void CubicShader::initializeShader() {
	compile(sCubicShaderVertSrc, sCubicShaderFragSrc);
	mParameter = glGetAttribLocation(progHandle(), "parameter");
	mSide = glGetAttribLocation(progHandle(), "side");
	mMvp = glGetUniformLocation(progHandle(), "mvp");
	mColor = glGetUniformLocation(progHandle(), "color");
	mPositionMatrix = glGetUniformLocation(progHandle(), "positionMatrix");
	mStrokeMatrix = glGetUniformLocation(progHandle(), "strokeMatrix");
	mStrokeVector = glGetUniformLocation(progHandle(), "strokeVector");
	mAttribBegin = &mParameter;
	mAttribEnd = &mSide+1;
}


static const char sCubicFillShaderVertSrc[] = 
	"uniform mediump mat4 positionMatrix;\n"
	"uniform mediump mat4 mvp;\n"
	"attribute mediump vec4 parameter;\n"
	"void main ()\n"
	"{\n"
	"  mediump vec4 tmpvar_1;\n"
	"  tmpvar_1.w = 1.0;\n"
	"  tmpvar_1.xyz = (positionMatrix * parameter).xyz;\n"
	"  mediump vec4 tmpvar_2;\n"
	"  tmpvar_2 = (mvp * tmpvar_1);\n"
	"  gl_Position = tmpvar_2;\n"
	"}\n";

static const char sCubicFillShaderFragSrc[] = 
	"uniform lowp vec4 color;\n"
	"void main ()\n"
	"{\n"
	"  gl_FragColor = color;\n"
	"}\n";

void CubicFillShader::initializeShader() {
	compile(sCubicFillShaderVertSrc, sCubicFillShaderFragSrc);
	mParameter = glGetAttribLocation(progHandle(), "parameter");
	mMvp = glGetUniformLocation(progHandle(), "mvp");
	mColor = glGetUniformLocation(progHandle(), "color");
	mPositionMatrix = glGetUniformLocation(progHandle(), "positionMatrix");
	mAttribBegin = &mParameter;
	mAttribEnd = &mParameter+1;
}


void SimpleMaterial::initializeMaterial(SimpleShader* pShader) {
	pShader->initializeShader();
	setShader(pShader);
	setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	setDepthNone();
	setTexture(0);
}

void ArcMaterial::initializeMaterial(ArcShader* pShader) {
	pShader->initializeShader();
	setShader(pShader);
	setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	setDepthNone();
	setTexture(0);
}

void CircleMaterial::initializeMaterial(CircleShader* pShader) {
	pShader->initializeShader();
	setShader(pShader);
	setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	setDepthNone();
	setTexture(0);
}

void CubicMaterial::initializeMaterial(CubicShader* pShader) {
	pShader->initializeShader();
	setShader(pShader);
	setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	setDepthNone();
	setTexture(0);
}

void CubicFillMaterial::initializeMaterial(CubicFillShader* pShader) {
	pShader->initializeShader();
	setShader(pShader);
	setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	setDepthNone();
	setTexture(0);
}

