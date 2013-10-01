#include "Blindr.h"

Blindr::Player::Player(World *world) : groundCount(0), facingRight(true), pressingLeft(false), pressingRight(false) {
	b2BodyDef bodyParams;
	bodyParams.type = b2_dynamicBody;
	bodyParams.position.Set( 0.5f * world->screenSize().x, 1);
	bodyParams.fixedRotation = true;
	bodyParams.userData = this;
	body = world->getSim()->CreateBody(&bodyParams);
	
	b2CircleShape hitbox;
	hitbox.m_radius = 0.5f;
	
	b2FixtureDef hitboxParams;
	hitboxParams.shape = &hitbox;
	hitboxParams.restitution= 0.05f;
	hitboxParams.friction = 0.01f;
	hitboxParams.filter.categoryBits = ctPlayer;
	hitboxParams.density = 1.0f;
	
	body->CreateFixture(&hitboxParams);
	
	b2CircleShape footShape;
	footShape.m_radius = 0.1f;
	footShape.m_p.Set(0, 0.5f);
	
	b2FixtureDef footParams;
	footParams.shape = &footShape;
	footParams.isSensor = true;
	footParams.filter.categoryBits = ctPlayerFoot;
	footParams.filter.maskBits = ctFloor;
	
	foot = body->CreateFixture(&footParams);
	
}

void Blindr::Player::jump() {
	if (grounded()) {
		Audio::playSample(assets->jump);
		body->ApplyLinearImpulse(b2Vec2(0, -PlayerJumpForce), body->GetPosition());
	}
	
}

void Blindr::Player::preTick() {
	b2Vec2 targetVelocity = body->GetLinearVelocity();

	if (pressingLeft) {
		targetVelocity.x = -PlayerRunForce;
	} else if (pressingRight) {
		targetVelocity.x = PlayerRunForce;
	} else {
		targetVelocity.x = 0;
	}
	
	body->ApplyForce(targetVelocity - body->GetLinearVelocity(), body->GetPosition());
}

void Blindr::Player::postTick() {

}

void Blindr::Player::draw() {
	vec2 pixelPosition = vec(PixelsPerMeter * body->GetPosition());
	int idleFrame = pingPong(int(Time::seconds() * PlayerIdleFPS), assets->camel->nframes);
	if (facingRight) {
		SpriteBatch::draw(assets->camel, pixelPosition + vec(4, -4), idleFrame);
	} else {
		SpriteBatch::drawFlippedH(assets->camel, pixelPosition + vec(-4, -4), idleFrame);
	}
}
