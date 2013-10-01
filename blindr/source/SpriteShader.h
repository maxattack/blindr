#pragma once
#include "Graphics.h"

//--------------------------------------------------------------------------------
// Generated by tools/export_shaders.py
//--------------------------------------------------------------------------------

class SpriteShader : public Graphics::Shader {
private:
	GLuint mPos;
	GLuint mTex;
	GLuint mColorMod;
	GLuint mMvp;
	GLuint mColorAdd;
	GLuint mAtlasImage;
	GLuint mAtlasSizeInv;
	
public:
	void initializeShader();

	
	GLuint posHandle() const { return mPos; };
	GLuint texHandle() const { return mTex; };
	GLuint colorModHandle() const { return mColorMod; };
	GLuint mvpHandle() const { return mMvp; };
	GLuint colorAddHandle() const { return mColorAdd; };
	GLuint atlasImageHandle() const { return mAtlasImage; };
	GLuint atlasSizeInvHandle() const { return mAtlasSizeInv; };
	
	void setColorMod(float x, float y, float z, float w) {
		glUniform4f(mColorMod, x, y, z, w);
	}

	void setMvp(const float* buf, bool transpose = 0) {
		glUniformMatrix4fv(mMvp, 1, transpose, buf);
	}

	void setColorAdd(float x, float y, float z, float w) {
		glUniform4f(mColorAdd, x, y, z, w);
	}

	void setAtlasSizeInv(float x, float y) {
		glUniform2f(mAtlasSizeInv, x, y);
	}

	void setAtlasImage(int activeTextureId) {
		glUniform1i(mAtlasImage, activeTextureId);
	}

	void setPosPointer(float *p, int stride=0) {
		glVertexAttribPointer(mPos, 2, GL_FLOAT, GL_FALSE, stride, p);
	}

	void setTexPointer(float *p, int stride=0) {
		glVertexAttribPointer(mTex, 2, GL_FLOAT, GL_FALSE, stride, p);
	}

};

