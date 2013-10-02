#include "Blindr.h"

Blindr::Debris::Debris(World *world) :
BodyUserdata(btDebris),
lastJuggleTime(-1),
juggleCount(0),
juggleNext(0),
next(0),
inJuggleList(false) {
	
	b2BodyDef bodyParams;
	bodyParams.type = b2_dynamicBody;
	bodyParams.gravityScale = 0.04f;
	bodyParams.position.x = randomValue(2, world->screenSize().x-2);
	bodyParams.position.y = world->getScrollMeters() - 1.0f;
	bodyParams.linearDamping = 1.25f;
	bodyParams.userData = this;
	body = world->getSim()->CreateBody(&bodyParams);
	
	b2CircleShape circleShape;
	circleShape.m_radius = 0.333f;
	b2FixtureDef fixParams;
	fixParams.shape = &circleShape;
	fixParams.restitution = 0.275f;
	fixParams.filter.categoryBits = ctDebris;
	fixParams.filter.maskBits = ctPlayer | ctWall;
	fixParams.density = 1.5f;
	body->CreateFixture(&fixParams);
}

bool Blindr::Debris::shouldCull() {
	float dy = body->GetPosition().y - world->getScrollMeters();
	return dy > world->screenSize().y + 1;
}

Blindr::Debris::~Debris() {
	world->getSim()->DestroyBody(body);
	body = 0;
}
		
void Blindr::Debris::draw() {
	SpriteBatch::draw(assets->bomb, vec(body->GetPosition()) * PixelsPerMeter - vec(0,2));
}

bool Blindr::Debris::didGetJuggled(b2Vec2 normal) {
	if (inJuggleList) {
		return false;
	}
	juggleNormal = normal;
	float prev = lastJuggleTime;
	lastJuggleTime = Time::seconds();
	if (lastJuggleTime - prev > 0.333f) {
		inJuggleList = true;
		return true;
	} else {
		return false;
	}
}

void Blindr::Debris::applyJuggle() {
	++juggleCount;

	b2Filter filter = body->GetFixtureList()->GetFilterData();
	filter.maskBits |= ctGazer;
	body->GetFixtureList()->SetFilterData(filter);

	float grav = 0.9f * (1-body->GetGravityScale());
	body->SetGravityScale(1-grav);
	body->SetLinearDamping(0.8f * body->GetLinearDamping());
	body->ApplyLinearImpulse(JuggleStrength * (b2Vec2(0,-juggleCount) + juggleNormal), body->GetPosition());
	
	inJuggleList = false;
}