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
#include <SDL.h>
#include "Assets.h"
#include "VectorMath.h"

#define COLOR_DIV (1.f/255.f)

struct Color {
	union {
		uint32_t argb;
		struct {
			// little endian
			uint8_t b, g, r, a;
		};
	};

	Color() {}
	Color(uint8_t ar, uint8_t ag, uint8_t ab, uint8_t aa=255) : b(ab), g(ag), r(ar), a(aa) {}
	Color(uint32_t aargb) : argb(aargb) {}

	float red() { return r * COLOR_DIV; }
	float green() { return g * COLOR_DIV; }
	float blue() { return b * COLOR_DIV; }
	float alpha() { return a * COLOR_DIV; }
	
	void toHSV(float *h, float *s, float *v);
};


inline Color rgb(uint32_t rgb) {
	return Color(0xff000000 | rgb);
}

inline Color rgb(float r, float g, float b) {
	return Color(255 * r, 255 * g, 255 * b);
}

inline Color rgba(float r, float g, float b, float a) {
	return Color(255 * r, 255 * g, 255 * b, 255 * a);
}

inline Color rgba(Color c, float a) {
	c.a = 255 * a;
	return c;
}

inline Color lerp(Color a, Color b, float u) {
	return rgba(
		lerp(a.red(), b.red(), u),
		lerp(a.green(), b.green(), u),
		lerp(a.blue(), b.blue(), u),
		lerp(a.alpha(), b.alpha(), u)
	);
}

Color hsv(float h, float s, float v);

namespace Graphics {

	void init(int pxSize, int width, int height, float zNear=-256, float zFar=256);
	void release();

	SDL_Window *window();
	
	inline vec2 windowSize() {
		int w,h;
		SDL_GetWindowSize(window(), &w, &h);
		return vec(w,h);
	}


	mat4 matrix();
	void setMatrix(const mat4& m);
	float pixelScale();
	vec2 canvasSize();
	
	class Shader {
	private:
		GLuint mVert;
		GLuint mFrag;
		GLuint mProg;

	protected:
		const GLuint *mAttribBegin;
		const GLuint *mAttribEnd;

	public:

		void compile(const char *vsrc, const char*fsrc);
		void release();

		void enableShader() {
			glUseProgram(mProg);
			for(const GLuint *p=mAttribBegin; p!=mAttribEnd; ++p) {
				glEnableVertexAttribArray(*p);
			}
		}
		
		void disableShader() {
			for(const GLuint *p=mAttribBegin; p!=mAttribEnd; ++p) {
				glDisableVertexAttribArray(*p);
			}
			glUseProgram(0);
		}

		GLuint vertHandle() const { return mVert; }
		GLuint fragHandle() const { return mFrag; }
		GLuint progHandle() const { return mProg; }
	};

	class Material {
	private:
		Shader *pShader;
		int mTexture;
		int mBlendSrc;
		int mBlendDst;
		int mDepthFunc;

	public:

		bool hasBlending() const { return mBlendSrc != -1; }
		bool hasDepthTesting() const { return mDepthFunc != -1; }
		bool hasShader() const { return pShader != 0; }
		bool hasTexture() const { return mTexture != 0; }
		
		Shader* shader() const { return pShader; }
		int textureHandle() const { return mTexture; }
		int blendSrc() const { return mBlendSrc; }
		int blendDst() const { return mBlendDst; }
		int depthFunc() const { return mDepthFunc; }

		void setBlendNone() { mBlendSrc = -1; }
		void setBlendFunc(GLuint src, GLuint dst) { mBlendSrc=src; mBlendDst=dst; }

		void setDepthNone() { mDepthFunc = -1; }
		void setDepthFunc(GLuint func) { mDepthFunc = func; }

		void setTexture(GLuint tex) { mTexture = tex; }
		void setShader(Shader *shader) { pShader = shader; }

		void clearSettings() {
			mTexture = 0;
			pShader = 0;
			mBlendSrc = -1;
			mBlendDst = -1;
			mDepthFunc = -1;
		}

		virtual void enable() {};
		virtual void disable() {};
	};

	void setMaterial(Material* p);

	inline bool isLoaded(TextureAsset *asset) { return asset->textureHandle != 0; }
	void loadTexture(TextureAsset *asset);
	void unloadTexture(TextureAsset *asset);

	// helpers

	struct ScopedTransform {
		mat4 prev;
		ScopedTransform(mat4 tform) : prev(matrix()) { setMatrix(matrix() * tform); }
		~ScopedTransform() { setMatrix(prev); }
	};
	
	inline int pingPong(int i, int n) {
		i  = i % (2 * n - 2);
		return i >= n ? 2 * (n-1) - i + 1 : i;
	}

	inline int wrap(int nframes, float t, int fps=12) {
		return int(t * fps) % nframes;
	}

}
