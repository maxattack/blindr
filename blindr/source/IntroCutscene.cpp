#include "Blindr.h"

void Blindr::World::drawBackground() {
	SpriteBatch::begin(assets->Background);
	SpriteBatch::draw(assets->background, vec(0,0));
	SpriteBatch::end();
	
	drawTilemap();

}

bool Blindr::World::simpleYield() {
	bool result = false;

	SDL_GL_SwapWindow(Graphics::window());
	Time::tick();
	SDL_Event ev;
	while(SDL_PollEvent(&ev)) {
		if (ev.type == SDL_QUIT || (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE)) {
			SDL_Quit();
			exit(0);
		} else {
			result |= (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_SPACE);
		}
	}
	return result;
}

bool Blindr::World::cutsceneYield() {	
	bool result = simpleYield();
	drawBackground();
	return result;
}

void Blindr::World::titleScene() {

	drawBackground();

	do {
		{
		Graphics::ScopedTransform push( translationMatrix(vec(0, -scrollMeters * PixelsPerMeter)) );
		SpriteBatch::begin(assets->Sprites);
		player->drawIdle();
		gazer->drawIntro(0);
		SpriteBatch::end();
		}
		
		SpriteBatch::begin(assets->Title);
		SpriteBatch::draw(assets->title, 0.5f * Graphics::canvasSize());
		SpriteBatch::end();
		
		SpriteBatch::drawLabelCentered(assets->flixel, "Press Space to Begin\n\nTheme: Blindness // Element: Juggler\nCode: xeW // Art: Blob // Music: Derris-Kharlan\nThis game was developed as part of Indie Speed Run 2013 (indiespeedrun.com)", vec(0.5f, 0.77f) * Graphics::canvasSize());
		
	} while(!cutsceneYield());
	
}

void Blindr::World::introCutscene() {
//
//	for(float t=0; t<1.0f; t+=Time::deltaSeconds()) {
//		{
//			Graphics::ScopedTransform push( translationMatrix(vec(0, -scrollMeters * PixelsPerMeter)) );
//			SpriteBatch::begin(assets->Sprites);
//			player->drawIdle();
//			gazer->drawIntro(0);
//			SpriteBatch::end();
//		}
//		cutsceneYield();
//	}

	
	for(float t=0; t<0.5f; t+=Time::deltaSeconds()) {
		float u = easeOut2(t/0.5f);
		{
			Graphics::ScopedTransform push( translationMatrix(vec(0, -scrollMeters * PixelsPerMeter)) );
			SpriteBatch::begin(assets->Sprites);
			player->drawIdle();
			gazer->drawIntro(u);
			SpriteBatch::end();
		}
		cutsceneYield();
	}
	
	// eye appears
	
	for(float t=0; t<1.0f; t+=Time::deltaSeconds()) {
		float u = t / 1.0f;
		{
			Graphics::ScopedTransform push( translationMatrix(vec(0, -scrollMeters * PixelsPerMeter)) );
			SpriteBatch::begin(assets->Sprites);
			player->drawIdle();
			gazer->drawRaw(easeOut4(u));
			SpriteBatch::end();
		}
		cutsceneYield();
	}
	
	
	
	// bomb falls on camera
	vec2 cp = vec(player->getBody()->GetPosition()) * PixelsPerMeter;
	
	for(float t=0.05f; t<0.8f; t+=Time::deltaSeconds()) {
		float u = 1-easeOut2(1-t/0.8f);
		{
			Graphics::ScopedTransform push( translationMatrix(vec(0, -scrollMeters * PixelsPerMeter)) );
			SpriteBatch::begin(assets->Sprites);
			player->drawIdle();
			
			float y0 = scrollMeters * PixelsPerMeter;
			float y1 = cp.y-10;
			float y = y0 + u * (y1 - y0);
			
			
			SpriteBatch::draw(assets->bomb, vec(cp.x, y));
			
			gazer->drawRaw();
			SpriteBatch::end();
		}
		cutsceneYield();
	}
	Audio::playSample(assets->juggle);
	
	// bounces up to eye
	vec2 gp = vec(gazer->getBody()->GetPosition()) * PixelsPerMeter;
	vec2 p1 = gp + vec(-5, 5);
	
	for(float t=0.0f; t<1.15f; t+=Time::deltaSeconds()) {
		float u = t / 1.15f;
		{
			Graphics::ScopedTransform push( translationMatrix(vec(0, -scrollMeters * PixelsPerMeter)) );
			SpriteBatch::begin(assets->Sprites);
			player->drawIdle();
			
			gazer->drawRaw();
			vec2 p0 = cp - vec(0, 10);
			
			SpriteBatch::draw(assets->bomb, p0 + u * (p1 - p0) - vec(0, 50 * parabola(u)));

			SpriteBatch::end();
		}
		cutsceneYield();
	}
	
	// explodes
	Audio::playSample(assets->expl);
	for(float t=0.0f; t<0.65f; t+=Time::deltaSeconds()) {
		float u = t / 0.65f;
		{
			Graphics::ScopedTransform push( translationMatrix(vec(0, -scrollMeters * PixelsPerMeter)) );
			SpriteBatch::begin(assets->Sprites);
			player->drawIdle();
			
			gazer->drawRaw(1 - parabola(u), true);
			
			int eyelidFrame = int(u * 3) % 2;
			SpriteBatch::draw(assets->gazer_eyelid, gp, eyelidFrame);
			
			int frame = int(u * assets->explosion->nframes);
			
			SpriteBatch::draw(assets->explosion, p1, frame);

			SpriteBatch::end();
		}
		cutsceneYield();
	}
	
	
	// show go
	for(float t=0; t<0.85f; t+=Time::deltaSeconds()) {
		float u = easeOut4(t / 0.85f);
		{
			Graphics::ScopedTransform push( translationMatrix(vec(0, -scrollMeters * PixelsPerMeter)) );
			SpriteBatch::begin(assets->Sprites);
			player->drawIdle();
			gazer->drawRaw();
			SpriteBatch::end();
		}
		
		SpriteBatch::setColorMod(rgba(1, 1, 1, u));
		SpriteBatch::begin(assets->HUD);
		SpriteBatch::draw(assets->go, 0.5f * Graphics::canvasSize() - (1-u) * vec(0, -64));
		SpriteBatch::end();
		SpriteBatch::setColorMod(rgba(1, 1, 1, 1));
		
		cutsceneYield();
	}
	
	for(float t=0; t<2.5f; t+=Time::deltaSeconds()) {
		{
			Graphics::ScopedTransform push( translationMatrix(vec(0, -scrollMeters * PixelsPerMeter)) );
			SpriteBatch::begin(assets->Sprites);
			player->drawIdle();
			gazer->drawRaw();
			SpriteBatch::end();
		}
		
		SpriteBatch::begin(assets->HUD);
		SpriteBatch::draw(assets->go, 0.5f * Graphics::canvasSize());
		SpriteBatch::end();
		
		cutsceneYield();
	}
}