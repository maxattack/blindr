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
#include "Math.h"

#define GLEW_STATIC 1
#include "glew.h"

#if TARGET_OS_IPHONE
#include "SDL_opengles2.h"
#else 
#include "SDL_opengl.h"
#endif

// namespace?
#define ASCII_BEGIN    32
#define ASCII_END      127

#define TEXTURE_FLAG_FILTER 0x1
#define TEXTURE_FLAG_REPEAT 0x2
#define TEXTURE_FLAG_LUM    0x4
#define TEXTURE_FLAG_RGB    0x8

struct TextureAsset {
	int32_t w, h;
	uint32_t compressedSize;
	uint32_t textureHandle;
	uint32_t flags;
	void *compressedData;
	
	inline vec2 size() const { return vec(w,h); }
	inline int format() const { return GL_RGBA; }
//		if (flags & TEXTURE_FLAG_LUM) { return GL_LUMINANCE; }
//		else if (flags & TEXTURE_FLAG_RGB) { return GL_RGB; }
//		else { return GL_RGBA; }
//	}

};

struct FrameAsset {
	int32_t x, y, w, h, px, py, tposed;
	
	inline vec2 topLeft() const { return vec(x,y); }
	inline vec2 size() const { return vec(w,h); }
	inline vec2 pivot() const { return vec(px, py); }
};

struct ImageAsset {
	TextureAsset *texture;
	int32_t w, h, px, py, nframes;

	inline FrameAsset* frame(int i) {
		ASSERT(i >= 0 && i < nframes);
		return reinterpret_cast<FrameAsset*>(this+1) + i;
	}
	
	inline vec2 logicalSize() const { return vec(w,h); }
	inline vec2 logicalPivot() const { return vec(px, py); }
};

struct TilemapAsset {
	uint32_t tw, th, mw, mh, compressedSize;
	void *compressedData;
	TextureAsset tileAtlas;
	
	inline vec2 tileSize() const { return vec(tw, th); }
	inline vec2 mapSize() const { return vec(mw, mh); }
	inline vec2 pixelSize() const { return tileSize() * mapSize(); }
};


struct GlyphAsset {
	int32_t x, y, advance;
	
	inline vec2 pos() const { return vec(x,y); }
};

struct FontAsset {
	int32_t height;
	GlyphAsset glyphs[ASCII_END-ASCII_BEGIN];
	TextureAsset texture;
	
	GlyphAsset getGlyph(const char c) {
		ASSERT(c >= ASCII_BEGIN && c < ASCII_END);
		return glyphs[c-ASCII_BEGIN];
	}
};

struct SampleAsset {
	int32_t channelCount;
	int32_t sampleWidth;
	int32_t frequency;
	uint32_t size;
	uint32_t compressedSize;
	uint32_t bufferHandle;
	void *compressedData;
};

namespace Assets {

int load(const char *path, void **outData, size_t *outLength);
int unload(void *p, size_t length);

}
