#include "Graphics.h"
#include "Graphics.h"
#include <zlib.h>
#include <algorithm>

using namespace Graphics;

static mat4 mvpMatrix;
static SDL_Window* pWindow = 0;

mat4 frustrumProjectionMatrix(
							  float left, float right,
							  float bottom, float top,
							  float nearZ, float farZ
							  ) {
    float ral = right + left;
    float rsl = right - left;
    float tsb = top - bottom;
    float tab = top + bottom;
    float fan = farZ + nearZ;
    float fsn = farZ - nearZ;
    
    return mat(
			   2.0f * nearZ / rsl, 0.0f, 0.0f, 0.0f,
			   0.0f, 2.0f * nearZ / tsb, 0.0f, 0.0f,
			   ral / rsl, tab / tsb, -fan / fsn, -1.0f,
			   0.0f, 0.0f, (-2.0f * farZ * nearZ) / fsn, 0.0f
			   );
}

mat4 orthogonalProjectionMatrix(float left, float right, float bottom, float top, float znear, float zfar) {
	float ral = right + left;
	float rsl = right - left;
	float tab = top + bottom;
	float tsb = top - bottom;
	float fan = zfar + znear;
	float fsn = zfar - znear;
    return mat(
			   2.0f / rsl, 0.0f, 0.0f, 0.0f,
			   0.0f, 2.0f / tsb, 0.0f, 0.0f,
			   0.0f, 0.0f, -2.0f / fsn, 0.0f,
			   -ral / rsl, -tab / tsb, -fan / fsn, 1.0f
			   );
}

static vec2 sCanvasSize;
static float sPixelScale;

void Graphics::init(int pxSize, int width, int height, float zNear, float zFar) {
	// startup SDL video
	SDL_GL_SetAttribute(SDL_GL_RETAINED_BACKING, 1);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	pWindow = SDL_CreateWindow("BLINDR", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, pxSize*width, pxSize*height, SDL_WINDOW_OPENGL);
	
	SDL_GL_CreateContext(pWindow);
	
	// default mvp is just the canvas
	int w, h;
	SDL_GetWindowSize(pWindow, &w, &h);
	glViewport(0, 0, w, h);
	
	sCanvasSize = vec(width, width * h / float(w));
	sPixelScale = w / width;
	mvpMatrix = orthogonalProjectionMatrix(0, width, sCanvasSize.y, 0, zNear, zFar);
	
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(0);
	
	// default clear color it the Little Polygon blue
	Color cc = rgb(0x404d68);
	glClearColor(cc.red(), cc.green(), cc.blue(), 0);

}

float Graphics::pixelScale() {
	return sPixelScale;
}

SDL_Window *Graphics::window() {
	return pWindow;
}

mat4 Graphics::matrix() {
	return mvpMatrix;
}

void Graphics::setMatrix(const mat4& m) {
	mvpMatrix = m;
}

void Graphics::release() {
	// todo
}

//void Graphics::setCanvasMatrix(float pixelScale, float zNear, float zFar) {
//	int w, h;
//	SDL_GetWindowSize(pWindow, &w, &h);
//	mvpMatrix = orthogonalProjectionMatrix(0, w/pixelScale, h/pixelScale, 0, zNear, zFar);
//}

vec2 Graphics::canvasSize() {
	return sCanvasSize;
}

void Graphics::setMaterial(Material* p) {
	static Material *curr = 0;

	if (p != curr) {

		if (curr) { curr->disable(); }

		int texture = 0;
		int depthFunc = -1;
		int blendSrc = -1;
		int blendDst = -1;
		Shader *shader = 0;
		if (curr) {
			texture = curr->textureHandle();
			depthFunc = curr->depthFunc();
			blendSrc = curr->blendSrc();
			blendDst = curr->blendDst();
			shader = curr->shader();
		}

		int nextTexture = 0;
		int nextDepthFunc = -1;
		int nextBlendSrc = -1;
		int nextBlendDst = -1;
		Shader* nextShader = 0;
		if (p) {
			nextTexture = p->textureHandle();
			nextDepthFunc = p->depthFunc();
			nextBlendSrc = p->blendSrc();
			nextBlendDst = p->blendDst();
			nextShader = p->shader();
		}

		if (nextShader != shader) {
			if (shader) { shader->disableShader(); }
			if (nextShader) { nextShader->enableShader(); }
		}

		if (texture != nextTexture) {
			glBindTexture(GL_TEXTURE_2D, nextTexture);
		}

		if (blendSrc != nextBlendSrc) {
			if(nextBlendSrc == -1) {
				glDisable(GL_BLEND);
			} else {
				if (blendSrc == -1) {
					glEnable(GL_BLEND);
				}
				glBlendFunc(nextBlendSrc, nextBlendDst);
			}
		} else if (nextBlendSrc != -1 && blendDst != nextBlendDst) {
			glBlendFunc(nextBlendSrc, nextBlendDst);
		}

		if (depthFunc != nextDepthFunc) {
			if (nextDepthFunc == -1) {
				glDisable(GL_DEPTH_TEST);
			} else {
				if (depthFunc == -1) {
					glEnable(GL_DEPTH_TEST);
				}
				glDepthFunc(nextDepthFunc);
			}
		}
		
		if (p) { p->enable(); }
		curr = p;
	}
}

Color hsv(float h, float s, float v) {
	if(s > 0.001f) {
		h /= 60;
		int i = int(h);
		float f = h - i; // factorial part of h
		float p = v * ( 1 - s );
		float q = v * ( 1 - s * f );
		float t = v * ( 1 - s * ( 1 - f ) );
		switch( i ) {
			case 0: return rgb(v, t, p);
			case 1: return rgb(q, v, p);
			case 2: return rgb(p, v, t);
			case 3: return rgb(p, q, v);
			case 4: return rgb(t, p, v);
			default: return rgb(v, p, q);
		}
	} else {
		return rgb(v, v, v);
	}
}

void Color::toHSV(float *h, float *s, float *v) {
	float r = red();
	float g = green();
	float b = blue();
	float K = 0.f;
	if (g < b) {
		std::swap(g, b);
		K -= 1.f;
	}
	if (r < g) {
		std::swap(r, g);
		K = -2.f / 6.f - K;
	}
	
	float chroma = r - std::min(g, b);
	*h = 360.f * fabs(K + (g-b) / (6.f * chroma + 1e-20f));
	*s = chroma / (r + 1e-20f);
	*v = r;
}

void Graphics::loadTexture(TextureAsset *p) {
	if(p->textureHandle != 0) {
		glBindTexture(GL_TEXTURE_2D, p->textureHandle);
		return;
	}
	glGenTextures(1, &p->textureHandle);
	glBindTexture(GL_TEXTURE_2D, p->textureHandle);
	if (p->flags & TEXTURE_FLAG_FILTER) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	} else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	if (p->flags & TEXTURE_FLAG_REPEAT) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	} else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	uLongf size = 4 * p->w * p->h;
	Bytef *scratch = (Bytef *) malloc(size);
	int result = uncompress(scratch, &size, (const Bytef*)p->compressedData, p->compressedSize);
	ASSERT(result == Z_OK);
	int fmt = p->format();
	glTexImage2D(GL_TEXTURE_2D, 0, fmt, p->w, p->h, 0, fmt, GL_UNSIGNED_BYTE, scratch);
	free(scratch);
}

void Graphics::unloadTexture(TextureAsset *asset) {
	ASSERT(asset->textureHandle);
	glDeleteTextures(1, &asset->textureHandle);
}
