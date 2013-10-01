#include "Blindr.h"
#include <string>
#include <sstream>

Blindr::World::World() : sim(b2Vec2(0,40)), player(0), gazer(0), doDebugDraw(true) {
	
	sim.SetContactListener(this);
	
	dbgDraw.SetFlags( DebugDraw::e_shapeBit | DebugDraw::e_centerOfMassBit | DebugDraw::e_pairBit );
	sim.SetDebugDraw(&dbgDraw);

	b2Vec2 initialPlayerPos;
	initializeGeometry(&initialPlayerPos);
	
	player = new Player(this, initialPlayerPos);
	gazer = new Gazer(this);
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
				player->leftPressed();
				break;
			
			case SDLK_RIGHT:
				player->rightPressed();
				break;
			
			case SDLK_SPACE:
				player->jump();
				break;
				
			case SDLK_TAB:
				doDebugDraw = !doDebugDraw;
				break;
				
			}
			
			
			break;
		
		case SDL_KEYUP:
			switch(event.key.keysym.sym) {
			
			case SDLK_LEFT:
				player->leftReleased();
				break;
				
			case SDLK_RIGHT:
				player->rightReleased();
				break;
			
			}
		
			break;
			
		}
	}
}

void Blindr::World::run() {

	SpriteBatch::loadTilemap(assets->level);
	
	for(;;) {
		handleEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		scrollMeters -= ScrollMetersPerSecond * Time::deltaSeconds();
		
		player->preTick();
//		gazer->preTick();
		sim.Step(Time::deltaSeconds(), 8, 8);
		player->postTick();
//		gazer->postTick();
		
		SpriteBatch::begin(assets->Background);
		SpriteBatch::draw(assets->background, vec(0,0));
		SpriteBatch::end();
		
		float y = scrollMeters * PixelsPerMeter;
		
		SpriteBatch::drawTilemap(vec(TilemapOffsetX,y));
		
		{
			Graphics::ScopedTransform push( translationMatrix(vec(0, -scrollMeters * PixelsPerMeter)) );
		
			SpriteBatch::begin(assets->Sprites);
			player->draw();
//			gazer->draw();
			SpriteBatch::end();
		
			if (doDebugDraw) { sim.DrawDebugData(); }
		}
			
		std::stringstream msg;
		msg << "Y = " << int(scrollMeters) << "m";
		SpriteBatch::drawLabel(assets->flixel, msg.str().c_str(), vec(0,0));
		
		SDL_GL_SwapWindow(Graphics::window());
	}
	
}

void Blindr::World::BeginContact(b2Contact *contact) {

	// did the player enter contact with the ground?
	if (contact->GetFixtureA() == player->getFoot() || contact->GetFixtureB() == player->getFoot()) {
		player->incrementGroundCount();
	}
	
}

void Blindr::World::EndContact(b2Contact *contact) {
	
	// did the player lose contact with the ground
	if (contact->GetFixtureA() == player->getFoot() || contact->GetFixtureB() == player->getFoot()) {
		player->decrementGroundCount();
	}

}

void Blindr::World::PreSolve(b2Contact *contact, const b2Manifold *oldManifold) {
	
	// file collisions between "Player" and "Floor" if the player is travelling up
	if (contact->GetFixtureA() == player->getHitbox()) {
		checkForCloudPlatform(contact->GetFixtureB(), contact);
	} else if (contact->GetFixtureB() == player->getHitbox()) {
		checkForCloudPlatform(contact->GetFixtureA(), contact);
	}
}

void Blindr::World::PostSolve(b2Contact *contact, const b2ContactImpulse *impulse) {

}

void Blindr::World::checkForCloudPlatform(b2Fixture *fixture, b2Contact *contact) {
	float py = player->getBody()->GetPosition().y + PlayerHalfHeight;
	float fy = fixture->GetBody()->GetPosition().y;
	if (py > fy && (fixture->GetFilterData().categoryBits && ctFloor) != 0) {
		contact->SetEnabled(false);
	}
	
}

inline b2Vec2 tileToPosition(int tx, int ty) {
	return b2Vec2(tx - TilemapOffsetX * MetersPerPixel, ty);
}

void Blindr::World::initializeGeometry(b2Vec2 *outPlayerPosition) {
	int lowestSeen = 0;

	scrollMeters = kLevelH - screenSize().y;
	
	for(int i=0; i<kFloorCount; ++i) {
		FloorData data = kFloors[i];
		createFloor(tileToPosition(data.tx, data.ty), tileToPosition(data.tx+data.tw, data.ty));

		// position the player over the lowest floor
		if (data.ty > lowestSeen) {
			lowestSeen = data.ty;
			*outPlayerPosition = tileToPosition(data.tx, data.ty);
			outPlayerPosition->x += 0.5f * data.tw;
		}
	}
	
	
}

