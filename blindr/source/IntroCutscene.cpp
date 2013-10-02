#include "Blindr.h"

bool Blindr::World::introYield() {
	
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
	
	SpriteBatch::begin(assets->Background);
	SpriteBatch::draw(assets->background, vec(0,0));
	SpriteBatch::end();
	
	drawTilemap();
	return result;
}

void Blindr::World::titleScene() {

	do {
		{
		Graphics::ScopedTransform push( translationMatrix(vec(0, -scrollMeters * PixelsPerMeter)) );
		SpriteBatch::begin(assets->Sprites);
		player->drawIdle();
		gazer->drawIntro(0);
		SpriteBatch::end();
		}
		
		SpriteBatch::drawLabelCentered(assets->flixel, "Press Space to Begin\n\nTheme: Blindness // Element: Juggler\nCode: xeW // Art: Blob // Music: Derris-Kharlan", vec(0.5f, 0.8f) * Graphics::canvasSize());
		
	} while(!introYield());
	
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
//		introYield();
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
		introYield();
	}
	
	// eye appears
	
	for(float t=0; t<1.0f; t+=Time::deltaSeconds()) {
		float u = t / 1.0f;
		{
			Graphics::ScopedTransform push( translationMatrix(vec(0, -scrollMeters * PixelsPerMeter)) );
			SpriteBatch::begin(assets->Sprites);
			player->drawIdle();
			gazer->draw(easeOut4(u));
			SpriteBatch::end();
		}
		introYield();
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
			
			gazer->draw();
			SpriteBatch::end();
		}
		introYield();
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
			
			gazer->draw();
			vec2 p0 = cp - vec(0, 10);
			
			SpriteBatch::draw(assets->bomb, p0 + u * (p1 - p0) - vec(0, 50 * parabola(u)));

			SpriteBatch::end();
		}
		introYield();
	}
	
	// explodes
	Audio::playSample(assets->expl);
	for(float t=0.0f; t<0.65f; t+=Time::deltaSeconds()) {
		float u = t / 0.65f;
		{
			Graphics::ScopedTransform push( translationMatrix(vec(0, -scrollMeters * PixelsPerMeter)) );
			SpriteBatch::begin(assets->Sprites);
			player->drawIdle();
			
			gazer->draw(1 - parabola(u));
			
			int eyelidFrame = int(u * 3) % 2;
			SpriteBatch::draw(assets->gazer_eyelid, gp, eyelidFrame);
			
			int frame = int(u * assets->explosion->nframes);
			
			SpriteBatch::draw(assets->explosion, p1, frame);

			SpriteBatch::end();
		}
		introYield();
	}
	
	
	for(float t=0; t<0.5f; t+=Time::deltaSeconds()) {
		{
			Graphics::ScopedTransform push( translationMatrix(vec(0, -scrollMeters * PixelsPerMeter)) );
			SpriteBatch::begin(assets->Sprites);
			player->drawIdle();
			gazer->draw();
			SpriteBatch::end();
		}
		introYield();
	}
	
	
}