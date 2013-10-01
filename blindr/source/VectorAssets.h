//--------------------------------------------------------------------------------
// Generated by tools/export_assets.py
//--------------------------------------------------------------------------------

#pragma once
#include "Graphics.h"

class ArcShader : public Graphics::Shader {
private:
	GLuint mUnitAndNormal;
	GLuint mMvp;
	GLuint mColor;
	GLuint mStroke;
	GLuint mRadius;
	
public:
	void initializeShader();

	
	GLuint unitAndNormalHandle() const { return mUnitAndNormal; };
	GLuint mvpHandle() const { return mMvp; };
	GLuint colorHandle() const { return mColor; };
	GLuint strokeHandle() const { return mStroke; };
	GLuint radiusHandle() const { return mRadius; };
	
	void setMvp(const float* buf, bool transpose = 0) {
		glUniformMatrix4fv(mMvp, 1, transpose, buf);
	}

	void setColor(float x, float y, float z, float w) {
		glUniform4f(mColor, x, y, z, w);
	}

	void setStroke(float x) {
		glUniform1f(mStroke, x);
	}

	void setRadius(float x) {
		glUniform1f(mRadius, x);
	}

	void setUnitAndNormalPointer(float *p, int stride=0) {
		glVertexAttribPointer(mUnitAndNormal, 4, GL_FLOAT, GL_FALSE, stride, p);
	}

};


class CircleShader : public Graphics::Shader {
private:
	GLuint mUnit;
	GLuint mMvp;
	GLuint mColor;
	GLuint mRadius;
	
public:
	void initializeShader();

	
	GLuint unitHandle() const { return mUnit; };
	GLuint mvpHandle() const { return mMvp; };
	GLuint colorHandle() const { return mColor; };
	GLuint radiusHandle() const { return mRadius; };
	
	void setMvp(const float* buf, bool transpose = 0) {
		glUniformMatrix4fv(mMvp, 1, transpose, buf);
	}

	void setColor(float x, float y, float z, float w) {
		glUniform4f(mColor, x, y, z, w);
	}

	void setRadius(float x) {
		glUniform1f(mRadius, x);
	}

	void setUnitPointer(float *p, int stride=0) {
		glVertexAttribPointer(mUnit, 2, GL_FLOAT, GL_FALSE, stride, p);
	}

};


class CubicFillShader : public Graphics::Shader {
private:
	GLuint mParameter;
	GLuint mMvp;
	GLuint mColor;
	GLuint mPositionMatrix;
	
public:
	void initializeShader();

	
	GLuint parameterHandle() const { return mParameter; };
	GLuint mvpHandle() const { return mMvp; };
	GLuint colorHandle() const { return mColor; };
	GLuint positionMatrixHandle() const { return mPositionMatrix; };
	
	void setMvp(const float* buf, bool transpose = 0) {
		glUniformMatrix4fv(mMvp, 1, transpose, buf);
	}

	void setColor(float x, float y, float z, float w) {
		glUniform4f(mColor, x, y, z, w);
	}

	void setPositionMatrix(const float* buf, bool transpose = 0) {
		glUniformMatrix4fv(mPositionMatrix, 1, transpose, buf);
	}

	void setParameterPointer(float *p, int stride=0) {
		glVertexAttribPointer(mParameter, 4, GL_FLOAT, GL_FALSE, stride, p);
	}

};


class CubicShader : public Graphics::Shader {
private:
	GLuint mParameter;
	GLuint mSide;
	GLuint mMvp;
	GLuint mColor;
	GLuint mPositionMatrix;
	GLuint mStrokeMatrix;
	GLuint mStrokeVector;
	
public:
	void initializeShader();

	
	GLuint parameterHandle() const { return mParameter; };
	GLuint sideHandle() const { return mSide; };
	GLuint mvpHandle() const { return mMvp; };
	GLuint colorHandle() const { return mColor; };
	GLuint positionMatrixHandle() const { return mPositionMatrix; };
	GLuint strokeMatrixHandle() const { return mStrokeMatrix; };
	GLuint strokeVectorHandle() const { return mStrokeVector; };
	
	void setMvp(const float* buf, bool transpose = 0) {
		glUniformMatrix4fv(mMvp, 1, transpose, buf);
	}

	void setColor(float x, float y, float z, float w) {
		glUniform4f(mColor, x, y, z, w);
	}

	void setPositionMatrix(const float* buf, bool transpose = 0) {
		glUniformMatrix4fv(mPositionMatrix, 1, transpose, buf);
	}

	void setStrokeMatrix(const float* buf, bool transpose = 0) {
		glUniformMatrix4fv(mStrokeMatrix, 1, transpose, buf);
	}

	void setStrokeVector(float x, float y, float z, float w) {
		glUniform4f(mStrokeVector, x, y, z, w);
	}

	void setParameterPointer(float *p, int stride=0) {
		glVertexAttribPointer(mParameter, 4, GL_FLOAT, GL_FALSE, stride, p);
	}

	void setSidePointer(float *p, int stride=0) {
		glVertexAttribPointer(mSide, 1, GL_FLOAT, GL_FALSE, stride, p);
	}

};


class ArcMaterial : public Graphics::Material {
public:
	void initializeMaterial(ArcShader* shader);
};

class CircleMaterial : public Graphics::Material {
public:
	void initializeMaterial(CircleShader* shader);
};

class CubicMaterial : public Graphics::Material {
public:
	void initializeMaterial(CubicShader* shader);
};

class CubicFillMaterial : public Graphics::Material {
public:
	void initializeMaterial(CubicFillShader* shader);
};

