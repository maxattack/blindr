#include "Blindr.h"

Blindr::World::World() : sim(b2Vec2(0,40)), player(this) {
	
	sim.SetContactListener(this);
	
	dbgDraw.SetFlags( DebugDraw::e_shapeBit | DebugDraw::e_centerOfMassBit | DebugDraw::e_pairBit );
	sim.SetDebugDraw(&dbgDraw);
	
	b2BodyDef floorParams;
	floorParams.position.Set(0, screenSize().y);
	b2Body *floor = sim.CreateBody(&floorParams);
	
	b2EdgeShape edgeShape;
	edgeShape.Set(b2Vec2(0,0), b2Vec2(screenSize().x, 0));
	
	b2FixtureDef edgeParams;
	edgeParams.shape = &edgeShape;
	edgeParams.filter.categoryBits = ctFloor;
	
	floor->CreateFixture(&edgeParams);
	
	
}

void Blindr::World::run() {
	
	bool drawDebug = true;
	
	for(;;) {
		Time::tick();
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			switch(event.type) {
				
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				
				case SDLK_LEFT:
					player.leftPressed();
					break;
				
				case SDLK_RIGHT:
					player.rightPressed();
					break;
				
				case SDLK_SPACE:
					player.jump();
					break;
					
				case SDLK_p:
					drawDebug = !drawDebug;
				}
				
				
				break;
			
			case SDL_KEYUP:
				switch(event.key.keysym.sym) {
				
				case SDLK_LEFT:
					player.leftReleased();
					break;
					
				case SDLK_RIGHT:
					player.rightReleased();
					break;
				
				}
			
				break;
				
			}
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		player.preTick();
		sim.Step(Time::deltaSeconds(), 8, 8);
		player.postTick();
		
		SpriteBatch::begin(assets->camel->texture);
		player.draw();
		SpriteBatch::end();
		
		if (drawDebug) {
			Graphics::ScopedTransform push(attitudeMatrix(vec(PixelsPerMeter, 0)));
			sim.DrawDebugData();
		}
		
		SpriteBatch::drawLabel(assets->flixel, "Blindr 0.1", vec(0,0));
		
		SDL_GL_SwapWindow(Graphics::window());
	}
	
}

void Blindr::World::BeginContact(b2Contact *contact) {

	// did the player enter contact with the ground?
	if (contact->GetFixtureA() == player.getFoot() || contact->GetFixtureB() == player.getFoot()) {
		player.incrementGroundCount();
	}
	
}

void Blindr::World::EndContact(b2Contact *contact) {
	
	// did the player lose contact with the ground
	if (contact->GetFixtureA() == player.getFoot() || contact->GetFixtureB() == player.getFoot()) {
		player.decrementGroundCount();
	}

}

void Blindr::World::PreSolve(b2Contact *contact, const b2Manifold *oldManifold) {

}

void Blindr::World::PostSolve(b2Contact *contact, const b2ContactImpulse *impulse) {

}


