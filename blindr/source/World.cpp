#include "Blindr.h"

Blindr::World::World() : sim(b2Vec2(0,40)), player(this), doDebugDraw(true) {
	
	sim.SetContactListener(this);
	
	dbgDraw.SetFlags( DebugDraw::e_shapeBit | DebugDraw::e_centerOfMassBit | DebugDraw::e_pairBit );
	sim.SetDebugDraw(&dbgDraw);

	b2Vec2 ss = screenSize();
	
	createFloor(b2Vec2(0, ss.y), ss);
	createFloor(b2Vec2(5, 0.85f * ss.y), b2Vec2(ss.x-5, 0.85f * ss.y));
	
	
	
}

b2Body *Blindr::World::createFloor(b2Vec2 p0, b2Vec2 p1) {
	b2BodyDef floorParams;
	floorParams.position = p0;
	b2Body *floor = sim.CreateBody(&floorParams);
	
	b2EdgeShape edgeShape;
	edgeShape.Set(b2Vec2(0,0), p1 - p0);
	
	b2FixtureDef edgeParams;
	edgeParams.shape = &edgeShape;
	edgeParams.filter.categoryBits = ctFloor;
	
	floor->CreateFixture(&edgeParams);
	return floor;
}

void Blindr::World::handleEvents() {
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
				
			case SDLK_TAB:
				doDebugDraw = !doDebugDraw;
				break;
				
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
}

void Blindr::World::run() {
	
	for(;;) {
		handleEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		player.preTick();
		sim.Step(Time::deltaSeconds(), 8, 8);
		player.postTick();
		
		SpriteBatch::begin(assets->camel->texture);
		player.draw();
		SpriteBatch::end();
		
		if (doDebugDraw) {
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
	
	// file collisions between "Player" and "Floor" if the player is travelling up
	if (contact->GetFixtureA() == player.getHitbox()) {
		checkForCloudPlatform(contact->GetFixtureB(), contact);
	} else if (contact->GetFixtureB() == player.getHitbox()) {
		checkForCloudPlatform(contact->GetFixtureA(), contact);
	}
}

void Blindr::World::PostSolve(b2Contact *contact, const b2ContactImpulse *impulse) {

}

void Blindr::World::checkForCloudPlatform(b2Fixture *fixture, b2Contact *contact) {
	float py = player.getBody()->GetPosition().y + PlayerHalfHeight;
	float fy = fixture->GetBody()->GetPosition().y;
	if (py > fy && (fixture->GetFilterData().categoryBits && ctFloor) != 0) {
		contact->SetEnabled(false);
	}
	
}

