#include "Blindr.h"

Blindr::Debris::Debris() : BodyUserdata(btDebris) {
	
}

Blindr::Debris& Blindr::Debris::init() {
	
	b2BodyDef bodyParams;
	bodyParams.type = b2_dynamicBody;
	bodyParams.gravityScale = 0.025f;
	bodyParams.position.x = randomValue(2, world->screenSize().x-2);
	bodyParams.position.y = world->getScrollMeters() - 1.0f;
	bodyParams.linearDamping = 1.0f;
	body = world->getSim()->CreateBody(&bodyParams);
	
	b2CircleShape circleShape;
	circleShape.m_radius = 0.25f;
	b2FixtureDef fixParams;
	fixParams.shape = &circleShape;
	fixParams.restitution = 0.35f;
	fixParams.filter.categoryBits = ctDebris;
	fixParams.filter.maskBits = ctPlayer | ctWall;
	fixParams.density = 5.0f;
	body->CreateFixture(&fixParams);
	
	
	return *this;
}

Blindr::Debris& Blindr::Debris::release() {
	return *this;
}
		
void Blindr::Debris::draw() {

}
