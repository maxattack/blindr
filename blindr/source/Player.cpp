#include "Blindr.h"

Blindr::Player::Player(World *world) : groundCount(0), facingRight(true), pressingLeft(false), pressingRight(false) {

	b2BodyDef bodyParams;
	bodyParams.type = b2_dynamicBody;
	bodyParams.position.Set( 0.5f * world->screenSize().x, 1);
	bodyParams.fixedRotation = true;
	bodyParams.userData = this;
	body = world->getSim()->CreateBody(&bodyParams);
	
	
	b2PolygonShape hitboxShape;
	hitboxShape.SetAsBox(PlayerHalfWidth, PlayerHalfHeight);
	
	b2FixtureDef hitboxParams;
	hitboxParams.shape = &hitboxShape;
	hitboxParams.restitution= 0.05f;
	hitboxParams.friction = 0.01f;
	hitboxParams.filter.categoryBits = ctPlayer;
	hitboxParams.density = 1.0f;
	
	hitbox = body->CreateFixture(&hitboxParams);
	
	b2PolygonShape footShape;
	footShape.SetAsBox(0.5f, 0.05f, b2Vec2(0, PlayerHalfHeight), 0);
	
	b2FixtureDef footParams;
	footParams.shape = &footShape;
	footParams.isSensor = true;
	footParams.filter.categoryBits = ctPlayerFoot;
	footParams.filter.maskBits = ctFloor;
	
	foot = body->CreateFixture(&footParams);
	
}

bool Blindr::Player::grounded() {
	if (groundCount > 0) {
		return abs(body->GetLinearVelocity().y) < 0.1f;
	} else {
		return false;
	}
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
	ImageAsset *asset = grounded() ? assets->camel : assets->camel_jump;
	vec2 pixelPosition = vec(PixelsPerMeter * body->GetPosition());
	int idleFrame = pingPong(int(Time::seconds() * PlayerIdleFPS), asset->nframes);
	if (facingRight) {
		SpriteBatch::draw(asset, pixelPosition + vec(6, -4), idleFrame);
		SpriteBatch::draw(assets->camel_hump, pixelPosition + vec(0, -10));
	} else {
		SpriteBatch::drawFlippedH(asset, pixelPosition + vec(-6, -4), idleFrame);
		SpriteBatch::drawFlippedH(assets->camel_hump, pixelPosition + vec(0, -10));
	}
}
