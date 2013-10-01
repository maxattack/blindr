// TYRANOFORCE
// Copyright (C) 2013 Frederic Tarabout and Max Kaufmann
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

#include "SpriteBatch.h"
#include "SpriteShader.h"
#include <zlib.h>

using namespace Graphics;

#define BATCH_CAPACITY   (128)
#define VERTS_PER_SPRITE (4)
#define ELEMS_PER_SPRITE (6)

static int gBatchCount = -1;
static TextureAsset *pAtlas = 0;
static GLuint hSpriteElements = 0;

struct SpriteVertex {
	vec2 pos;
	vec2 tex;
};

static SpriteVertex spriteVertices[ VERTS_PER_SPRITE*BATCH_CAPACITY ];

class SpriteMaterial : public Material, public SpriteShader {
public:
	
	void initializeMaterial() {
		clearSettings();
		initializeShader();
		setShader(this);
		setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	
	void enable() {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hSpriteElements);
		setPosPointer((float*)&(spriteVertices->pos), sizeof(SpriteVertex));
		setTexPointer((float*)&(spriteVertices->tex), sizeof(SpriteVertex));
	}
	
	void disable() {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	
};

static SpriteMaterial sbMaterial;

void SpriteBatch::init() {
	{ // plot sprite elements
		unsigned short *scratch = (unsigned short*) malloc(6*sizeof(short)*BATCH_CAPACITY);
		for(int i=0; i<BATCH_CAPACITY; ++i) {
			scratch[6*i+0] = 4*i;
			scratch[6*i+1] = 4*i+1;
			scratch[6*i+2] = 4*i+2;
			scratch[6*i+3] = 4*i+2;
			scratch[6*i+4] = 4*i+1;
			scratch[6*i+5] = 4*i+3;
		}
		glGenBuffers(1, &hSpriteElements);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hSpriteElements);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*sizeof(short)*BATCH_CAPACITY, scratch, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		free(scratch);
	}
	sbMaterial.initializeMaterial();
	setColorAdd(rgb(0x000000));
	setColorMod(Color(0xffffffff));
}

void SpriteBatch::setColorAdd(Color c) {
	ASSERT(gBatchCount == -1);
	setMaterial(&sbMaterial);
	sbMaterial.setColorAdd(c.red(), c.green(), c.blue(), 0);
}

void SpriteBatch::setColorMod(Color c) {
	ASSERT(gBatchCount == -1);
	setMaterial(&sbMaterial);
	sbMaterial.setColorMod(c.red(), c.green(), c.blue(), c.alpha());
}

void SpriteBatch::begin(TextureAsset *p) {
	ASSERT(gBatchCount == -1);
	ASSERT(p);
	ASSERT(pAtlas == 0);
	setMaterial(&sbMaterial);
	loadTexture(p);
	sbMaterial.setAtlasSizeInv(1.f/p->w, 1.f/p->h);
	sbMaterial.setMvp(matrix().m);
	pAtlas = p;
	gBatchCount = 0;
}

inline void plot(vec2 pos, vec2 tex) {
	spriteVertices[gBatchCount].pos = pos;
	spriteVertices[gBatchCount].tex = tex;
	gBatchCount++;
}


void SpriteBatch::drawTexture(TextureAsset *atlas, vec2 p) {
	begin(atlas);
	sbMaterial.setAtlasSizeInv(1.f, 1.f);
	plot(p, vec(0,0));
	plot(p+vec(0, atlas->h), vec(0, 1));
	plot(p+vec(atlas->w, 0), vec(1, 0));
	plot(p+atlas->size(),    vec(1, 1));
	end();
}

void SpriteBatch::draw(ImageAsset *pi, vec2 p, int fr) {
	ASSERT(gBatchCount >= 0);
	ASSERT(pi->texture == pAtlas);
	if (gBatchCount == BATCH_CAPACITY * VERTS_PER_SPRITE) {
		glDrawElements(GL_TRIANGLES, BATCH_CAPACITY * ELEMS_PER_SPRITE, GL_UNSIGNED_SHORT, 0);
		gBatchCount = 0;
	}
	FrameAsset *pf = pi->frame(fr);
	p -= pf->pivot();
	plot(p, pf->topLeft());
	if (pf->tposed == 0) {
		plot(p+vec(0, pf->h), vec(pf->x, pf->y+pf->h));
		plot(p+vec(pf->w, 0), vec(pf->x+pf->w, pf->y));
		plot(p+pf->size(),    vec(pf->x+pf->w, pf->y+pf->h));
	} else {
		plot(p+vec(0, pf->h), vec(pf->x+pf->h, pf->y));
		plot(p+vec(pf->w, 0), vec(pf->x, pf->y+pf->w));
		plot(p+pf->size(),    vec(pf->x+pf->h, pf->y+pf->w));
	}
}

void SpriteBatch::drawFlippedH(ImageAsset *pi, vec2 p, int fr) {
	ASSERT(gBatchCount >= 0);
	ASSERT(pi->texture == pAtlas);
	if (gBatchCount == BATCH_CAPACITY * VERTS_PER_SPRITE) {
		glDrawElements(GL_TRIANGLES, BATCH_CAPACITY * ELEMS_PER_SPRITE, GL_UNSIGNED_SHORT, 0);
		gBatchCount = 0;
	}
	FrameAsset *pf = pi->frame(fr);
	p.x += pf->px;
	p.y -= pf->py;
	plot(p, pf->topLeft());
	if (pf->tposed == 0) {
		plot(p+vec(0, pf->h),      vec(pf->x, pf->y+pf->h));
		plot(p+vec(-pf->w, 0),     vec(pf->x+pf->w, pf->y));
		plot(p+vec(-pf->w, pf->h), vec(pf->x+pf->w, pf->y+pf->h));
	} else {
		plot(p+vec(0, pf->h),      vec(pf->x+pf->h, pf->y));
		plot(p+vec(-pf->w, 0),     vec(pf->x, pf->y+pf->w));
		plot(p+vec(-pf->w, pf->h), vec(pf->x+pf->h, pf->y+pf->w));
	}
}

void SpriteBatch::end() {
	if (gBatchCount > 0) {
		glDrawElements(GL_TRIANGLES, ELEMS_PER_SPRITE*(gBatchCount>>2), GL_UNSIGNED_SHORT, 0);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	pAtlas = 0;
	gBatchCount = -1;
}

//------------------------------------------------------------------------------
// TEXT LABEL RENDERING
//------------------------------------------------------------------------------

inline void doPlotGlyph(GlyphAsset g, float x, float y, float h) {
	if (gBatchCount == BATCH_CAPACITY * VERTS_PER_SPRITE) {
		glDrawElements(GL_TRIANGLES, BATCH_CAPACITY * ELEMS_PER_SPRITE, GL_UNSIGNED_SHORT, 0);
		gBatchCount = 0;
	}
	plot(vec(x, y), vec(g.x, g.y));
	plot(vec(x,y+h), vec(g.x, g.y+h));
	plot(vec(x+g.advance, y), vec(g.x+g.advance, g.y));
	plot(vec(x+g.advance, y+h), vec(g.x+g.advance, g.y+h));
}

void SpriteBatch::drawLabel(FontAsset *font, const char*msg, vec2 p) {
	// utilize the batching system LIKE A BOSS
	begin(&font->texture);
	float px = p.x;
	float py = p.y;
	while(*msg) {
		if (*msg != '\n') {
			GlyphAsset g = font->getGlyph(*msg);
			doPlotGlyph(g, px, py, font->height);
			px += g.advance;
		} else {
			px = p.x;
			py += font->height;
		}
		++msg;
	}
	end();
}

const char* SpriteBatch::measureTextLine(FontAsset *font, const char *msg, int *outLength) {
	*outLength = 0;
	while(*msg && *msg != '\n') {
		*outLength += font->getGlyph(*msg).advance;
		++msg;
	}
	return msg;
}

void SpriteBatch::drawLabelCentered(FontAsset *font, const char *msg, vec2 p) {
	begin(&font->texture);
	float py = p.y;
	while(*msg) {
		int length;
		const char* next = measureTextLine(font, msg, &length);
		float px = p.x - (length>>1);
		while(msg != next) {
			GlyphAsset g = font->getGlyph(*msg);
			doPlotGlyph(g, px, py, font->height);
			px += g.advance;
			msg++;
		}
		if (*msg == '\n') {
			py += font->height;
			msg++;
		}
	}
	end();
}

//------------------------------------------------------------------------------
// TILEMAP RENDERING
//------------------------------------------------------------------------------

struct TilemapCoord { uint8_t x, y; };

static TilemapAsset *sTilemapAsset = 0;
static TilemapCoord *sTilemapCoords = 0;
static int latticeW, latticeH = 0;

void SpriteBatch::loadTilemap(TilemapAsset *tilemap) {
	sTilemapAsset = tilemap;
	vec2 cs = canvasSize() / tilemap->tileSize();
	latticeW = ceilf(cs.x) + 1;
	latticeH = ceilf(cs.y) + 1;
	
	uLongf size = 2 * tilemap->mw * tilemap->mh;
	Bytef *scratch = (Bytef *) malloc(size);
	int result = uncompress(scratch, &size, (const Bytef*)tilemap->compressedData, tilemap->compressedSize);
	ASSERT(result == Z_OK);
	sTilemapCoords = (TilemapCoord*) scratch;
}

inline void doPlotTile(int x, int y, TilemapCoord coord) {
	if (gBatchCount == BATCH_CAPACITY * VERTS_PER_SPRITE) {
		glDrawElements(GL_TRIANGLES, BATCH_CAPACITY * ELEMS_PER_SPRITE, GL_UNSIGNED_SHORT, 0);
		gBatchCount = 0;
	}

	vec2 p = vec(x * sTilemapAsset->tw, y * sTilemapAsset->th);
	vec2 uv = vec(sTilemapAsset->tw * coord.x, sTilemapAsset->th * coord.y);
	float tw = sTilemapAsset->tw;
	float th = sTilemapAsset->th;

	float ep = 0.01f;
	plot(p, uv);
	plot(p+vec(0,th+ep), uv+vec(0,th));
	plot(p+vec(tw+ep,0), uv+vec(tw,0));
	plot(p+vec(tw+ep,th+ep), uv+vec(tw,th));
}


void SpriteBatch::drawTilemap(vec2 viewOffset) {
	double k = 4 * Graphics::pixelScale();
	viewOffset.x = float(floor(k * viewOffset.x) / k);
	viewOffset.y = float(floor(k * viewOffset.y) / k);

	ASSERT(sTilemapAsset);
	//glDisable(GL_BLEND);
	//TODO: Skip empties

	int vox = int(viewOffset.x/sTilemapAsset->tw);
	int voy = int(viewOffset.y/sTilemapAsset->th);
	
	vec2 rem = vec(
		fmodf(viewOffset.x, sTilemapAsset->tw),
		fmodf(viewOffset.y, sTilemapAsset->th)
	);
	ScopedTransform pushMatrix(translationMatrix(-rem));
	
	begin(&sTilemapAsset->tileAtlas);
	
	
	for(int y=0; y<latticeH; ++y)
	for(int x=0; x<latticeW; ++x) {
		doPlotTile(x, y, sTilemapCoords[(y+voy) * sTilemapAsset->mw + (x+vox)]);
	}
	end();
	//glEnable(GL_BLEND);
}


void SpriteBatch::unloadTilemap() {
	// TODO
	ASSERT(0);
}

