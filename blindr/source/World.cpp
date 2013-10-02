#include "Blindr.h"
#include <string>
#include <sstream>

// helperzzz

inline b2Vec2 tileToPosition(int tx, int ty) {
	return b2Vec2(tx - TilemapOffsetX * MetersPerPixel, ty);
}


Blindr::World::World() :
sim(b2Vec2(0,40)),
player(0),
gazer(0),
headDebris(0),
headExpl(0),
doDebugDraw(false),
juggleHead(0),
didHit(false) {
	
	sim.SetContactListener(this);
	
	dbgDraw.SetFlags( DebugDraw::e_shapeBit | DebugDraw::e_pairBit );
	sim.SetDebugDraw(&dbgDraw);

	{
		b2BodyDef groundParams;
		groundParams.type = b2_staticBody;
		ground = sim.CreateBody(&groundParams);
	}

	b2Vec2 initialPlayerPos;
	initializeGeometry(&initialPlayerPos);
	createWalls();
	
	player = new Player(this, initialPlayerPos);
	gazer = new Gazer(this);

	
	timeToNextSpawn = expovariate(TimeBetweenSpawns);
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

void Blindr::World::createWalls() {
	b2EdgeShape wall;
	wall.Set(tileToPosition(2,0), tileToPosition(2, kLevelH));
	
	b2FixtureDef fixParams;
	fixParams.shape = &wall;
	fixParams.filter.categoryBits = ctWall;
	ground->CreateFixture(&fixParams);
	
	wall.Set(tileToPosition(kLevelW-2, 0) ,tileToPosition(kLevelW-2, kLevelH));
	ground->CreateFixture(&fixParams);
}

void Blindr::World::createFloor(b2Vec2 p0, b2Vec2 p1) {
	b2EdgeShape edgeShape;
	edgeShape.Set(p0, p1);
	
	b2FixtureDef edgeParams;
	edgeParams.shape = &edgeShape;
	edgeParams.filter.categoryBits = ctFloor;
	
	ground->CreateFixture(&edgeParams);
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
				
			case SDLK_ESCAPE:
				SDL_Quit();
				exit(0);
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
		
		case SDL_QUIT:
			SDL_Quit();
			exit(0);
			break;
		}
	}
}

void Blindr::World::drawTilemap() {
	SpriteBatch::drawTilemap(vec(TilemapOffsetX,scrollMeters * PixelsPerMeter));
}

void Blindr::World::run() {
	SpriteBatch::loadTilemap(assets->level);

	// hack: init the iris
	gazer->postTick();
	
	//titleScene();
	//introCutscene();
	
	for(;;) {
		handleEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		scrollMeters -= ScrollMetersPerSecond * Time::deltaSeconds();
		
		// update scene
		timeToNextSpawn -= Time::deltaSeconds();
		if (timeToNextSpawn < 0.0f) {
			timeToNextSpawn += expovariate(TimeBetweenSpawns);
			Debris *deb = new Debris(this);
			deb->next = headDebris;
			if (headDebris) { headDebris->prev = deb; }
			headDebris = deb;
		}
		player->preTick();
		gazer->preTick();
		sim.Step(Time::deltaSeconds(), 8, 8);
		handleContacts();
		handleGazerHit();
		player->postTick();
		gazer->postTick();
		
		// draw scene
		
		SpriteBatch::begin(assets->Background);
		SpriteBatch::draw(assets->background, vec(0,0));
		SpriteBatch::end();
		
		drawTilemap();
		
		{
			Graphics::ScopedTransform push( translationMatrix(vec(0, -scrollMeters * PixelsPerMeter)) );
		
			SpriteBatch::begin(assets->Sprites);
			for(Debris *p = headDebris; p; ) {
				Debris *deb = p;
				p = p->next;
				if (deb->shouldCull()) {
					if (headDebris == deb) { headDebris = deb->next; }
					if (deb->next) { deb->next->prev = deb->prev; }
					if (deb->prev) { deb->prev->next = deb->next; }
					delete deb;
				} else {
					deb->draw();
				}
			}
			
			player->draw();
			gazer->draw();
			for (Explosion *p = headExpl; p; ) {
				Explosion *expl = p;
				p = p->next;
				if (expl->draw()) {
					if (headExpl == expl) { headExpl = headExpl->next; }
					if (expl->next) { expl->next->prev = expl->prev; }
					if (expl->prev) { expl->prev->next = expl->next; }
					delete expl;
				}
			}
			SpriteBatch::end();
		
			if (doDebugDraw) { sim.DrawDebugData(); }
		}
		
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

	// player collision?
	if (player->isHitbox(contact->GetFixtureA())) {
		onPlayerCollisionPreSolve(contact->GetFixtureB(), contact);
	} else if (player->isHitbox(contact->GetFixtureB())) {
		onPlayerCollisionPreSolve(contact->GetFixtureA(), contact);
	}
	
}

void Blindr::World::PostSolve(b2Contact *contact, const b2ContactImpulse *impulse) {
	// player collision?
	if (player->isHitbox(contact->GetFixtureA())) {
		onPlayerCollisionPostSolve(contact->GetFixtureB(), contact);
	} else if (player->isHitbox(contact->GetFixtureB())) {
		onPlayerCollisionPostSolve(contact->GetFixtureA(), contact);
	}
}

void Blindr::World::onPlayerCollisionPreSolve(b2Fixture *fixture, b2Contact *contact) {
	if (fixture->GetFilterData().categoryBits == ctFloor) {
		checkForCloudPlatform(fixture, contact);
	}
}
	
void Blindr::World::onPlayerCollisionPostSolve(b2Fixture *fixture, b2Contact *contact) {
	if (fixture->GetFilterData().categoryBits == ctDebris) {
		checkForJuggle(fixture, contact);
	}
}

void Blindr::World::checkForCloudPlatform(b2Fixture *fixture, b2Contact *contact) {
	float py = player->getBody()->GetPosition().y + PlayerHalfHeight;
	b2WorldManifold worldMan;
	contact->GetWorldManifold(&worldMan);
	float fy = worldMan.points[0].y;
	if (py > fy) {
		contact->SetEnabled(false);
	}
}

void Blindr::World::checkForJuggle(b2Fixture *fixture, b2Contact *contact) {
	b2WorldManifold worldMan;
	contact->GetWorldManifold(&worldMan);
	
	// normal points from A->B.  We want the direction from the player to the debris
	b2Vec2 normal = worldMan.normal;
	if (fixture == contact->GetFixtureA()) {
		normal = -normal;
	}
	// threshold for a juggle is "at least a little uppish"
	if (normal.y < -0.1f) {
		Debris *deb = (Debris*) fixture->GetBody()->GetUserData();
		if (deb->didGetJuggled(normal)) {
			deb->juggleNext = juggleHead;
			juggleHead = deb;
		}
	} else {
		didHit = true;
	}
	
}

void Blindr::World::handleContacts() {
	if (juggleHead) {
		Audio::playSample(assets->juggle);
	} else if (didHit) {
		Audio::playSample(assets->hit);
	}
	for(Debris *d=juggleHead; d; d=d->juggleNext) {
		d->applyJuggle();
	}
	juggleHead = 0;
	didHit = false;
}

void Blindr::World::handleGazerHit() {
	Debris *deb = gazer->findCollidingDebris();
	if (!deb) { return; }
		
	
	vec2 pos = vec(deb->getBody()->GetPosition()) * PixelsPerMeter;
	
	if (headDebris == deb) { headDebris = deb->next; }
	if (deb->next) { deb->next->prev = deb->prev; }
	if (deb->prev) { deb->prev->next = deb->next; }
	delete deb;
	
	Audio::playSample(assets->expl);
	gazer->blink();
	
	
	Explosion *expl = new Explosion(pos);
	expl->next = headExpl;
	if (headExpl) { headExpl->prev = expl; }
	headExpl = expl;
	
}

Blindr::World::~World() {
	for(Debris* p=headDebris; p; ) {
		Debris *deb = p;
		p = p->next;
		delete deb;
	}
	for(Explosion *p=headExpl; p; ) {
		Explosion *e = p;
		p = p->next;
		delete e;
	}
	delete gazer;
	delete player;
}
