
#include "Blindr.h"

void Blindr::World::outroCutscene() {

	drawBackground();

	if (player->dead()) {
		// fade to red
		
		Audio::playSample(assets->expl);
		for(float t=0; t<0.666f; t+=Time::deltaSeconds()) {
			float u = easeOut2(t/0.666f);
			VectorGraphics::fillScreen(rgba(1, 0.1f, 0.1f, u));
			cutsceneYield();
		}
		
		for(float t=0; t<0.666f; t+=Time::deltaSeconds()) {
			float u = 1-t/0.666f;
			VectorGraphics::fillScreen(rgb(u*1, u*0.1f, u*0.1f));
			simpleYield();
		}

		for(float t=0; t<0.666f; t+=Time::deltaSeconds()) {
			VectorGraphics::fillScreen(rgb(0, 0, 0));
			simpleYield();
		}
		
		do {
			VectorGraphics::fillScreen(rgb(0, 0, 0));
			SpriteBatch::begin(assets->Sprites);
			int frame = Graphics::pingPong(int(PlayerIdleFPS * Time::seconds()), assets->camel->nframes);
			SpriteBatch::draw(assets->camel, 0.5f * Graphics::canvasSize(), frame);
			SpriteBatch::end();
			SpriteBatch::drawLabelCentered(assets->flixel, "Next Time, Camel\n\nPress Space to Try Again", vec(0.5f, 0.66f) * Graphics::canvasSize());
		} while(!simpleYield());
		
	} else {
		
		// wait for blink to finish
		bool gazerDone = false;
		while(!gazerDone) {
		
			{
			Graphics::ScopedTransform push( translationMatrix(vec(0, -scrollMeters * PixelsPerMeter)) );
			SpriteBatch::begin(assets->Sprites);
			gazerDone = gazer->drawOutro();
			SpriteBatch::end();
			
			}
			cutsceneYield();
		}
		
		for(float t=0; t<1.5f; t+=Time::deltaSeconds()) {
			{
			Graphics::ScopedTransform push( translationMatrix(vec(0, -scrollMeters * PixelsPerMeter)) );
			SpriteBatch::begin(assets->Sprites);
			gazer->drawOutro();
			SpriteBatch::end();
			}
			
			VectorGraphics::fillScreen(rgba(0.01f, 0.01f, 0.1f, easeOut2(t/1.5f)));
			
			cutsceneYield();
		}
		
		do {
			VectorGraphics::fillScreen(rgb(0.01f, 0.01f, 0.1f));
			SpriteBatch::begin(assets->Sprites);
			int frame = Graphics::pingPong(int(PlayerIdleFPS * Time::seconds()), assets->camel->nframes);
			SpriteBatch::draw(assets->camel, 0.5f * Graphics::canvasSize(), frame);
			SpriteBatch::end();
			
			SpriteBatch::drawLabelCentered(assets->flixel, "WINNERRR!\n\nPress Space to Play Again", vec(0.5f, 0.66f) * Graphics::canvasSize());
			
			
		} while (!cutsceneYield());
		
	}

}
