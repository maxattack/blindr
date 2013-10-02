#include "Blindr.h"

Blindr::Debris::Debris() : BodyUserdata(btDebris) {
	
}

Blindr::Debris& Blindr::Debris::init() {
	
	lastJuggleTime = -1;
	juggleCount = 0;
	next = 0;
	
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
		
	return *this;
}

bool Blindr::Debris::shouldCull() {
	float dy = body->GetPosition().y - world->getScrollMeters();
	return dy > world->screenSize().y + 1;
}

Blindr::Debris& Blindr::Debris::release() {
	world->getSim()->DestroyBody(body);
	body = 0;
	return *this;
}
		
void Blindr::Debris::draw() {
	SpriteBatch::draw(assets->bomb, vec(body->GetPosition()) * PixelsPerMeter - vec(0,2));
}

bool Blindr::Debris::didGetJuggled(b2Vec2 normal) {
	juggleNormal = normal;
	float prev = lastJuggleTime;
	lastJuggleTime = Time::seconds();
	return lastJuggleTime - prev > 0.333f;
}

void Blindr::Debris::applyJuggle() {
	++juggleCount;

	float grav = 0.9f * (1-body->GetGravityScale());
	body->SetGravityScale(1-grav);
	body->SetLinearDamping(0.8f * body->GetLinearDamping());
	body->ApplyLinearImpulse(JuggleStrength * (b2Vec2(0,-juggleCount) + juggleNormal), body->GetPosition());
}