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
#include "Graphics.h"
#include "Assets.h"
#include "SpriteShader.h"

namespace SpriteBatch {
	void init();

	void setColorAdd(Color c);
	void setColorMod(Color c);
	
	// mainly for asset debugging
	void drawTexture(TextureAsset *atlas, vec2 p);

	void begin(TextureAsset *atlas);
	void draw(ImageAsset *image, vec2 p, int frame=0);
	void drawFlippedH(ImageAsset *image, vec2 p, int frame=0);
	void drawScaled(ImageAsset *image, vec2 p, vec2 s, int frame=0);
	void end();

	// internally call begin/end
	void drawLabel(FontAsset *font, const char *msg, vec2 p);
	const char *measureTextLine(FontAsset *font, const char *line, int *outLength);
	void drawLabelCentered(FontAsset *font, const char *msg, vec2 p);

	// internally calls begin/end
	void loadTilemap(TilemapAsset *tilemap);
	void drawTilemap(vec2 viewOffset);
	void unloadTilemap();
	
}
