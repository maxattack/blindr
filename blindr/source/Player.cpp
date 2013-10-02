#include "Blindr.h"

Blindr::Player::Player(World *world, b2Vec2 playerPos) :
BodyUserdata(btPlayer),
groundCount(0),
facingRight(true),
pressingLeft(false),
pressingRight(false) {

	b2BodyDef bodyParams;
	bodyParams.type = b2_dynamicBody;
	bodyParams.position = playerPos;
	bodyParams.position.y -= 0.5f;
	bodyParams.fixedRotation = true;
	bodyParams.userData = this;
	body = world->getSim()->CreateBody(&bodyParams);
	
	b2PolygonShape hitboxShape;
	hitboxShape.SetAsBox(PlayerHalfWidth, 0.5f * PlayerHalfHeight, b2Vec2(0, 0.5f * PlayerHalfHeight), 0);
	
	b2FixtureDef hitboxParams;
	hitboxParams.shape = &hitboxShape;
	hitboxParams.restitution= 0.05f;
	hitboxParams.friction = 0.01f;
	hitboxParams.filter.categoryBits = ctPlayer;
	hitboxParams.density = 1.0f;
	body->CreateFixture(&hitboxParams);
	
	b2CircleShape humpShape;
	humpShape.m_radius = PlayerHalfWidth;
	hitboxParams.shape = &humpShape;
	body->CreateFixture(&hitboxParams);
	
	b2PolygonShape footShape;
	footShape.SetAsBox(0.99f * PlayerHalfWidth, 0.05f, b2Vec2(0, PlayerHalfHeight), 0);
	
	b2FixtureDef footParams;
	footParams.shape = &footShape;
	footParams.isSensor = true;
	footParams.filter.categoryBits = ctPlayerFoot;
	footParams.filter.maskBits = ctFloor;
	
	foot = body->CreateFixture(&footParams);
	
}

bool Blindr::Player::isHitbox(b2Fixture *fix) {
	return !fix->IsSensor() && fix->GetBody()->GetUserData() == this;
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
	
	ImageAsset *asset = assets->camel_jump;
	if (grounded()) {
		float vx = body->GetLinearVelocity().x;
		if (abs(vx) > 0.02f) {
			asset = assets->camel_walk;
		} else {
			asset = assets->camel;
		}
	}

	vec2 pixelPosition = vec(PixelsPerMeter * body->GetPosition());
	
	int frame = 0;
	int humpOff = 0;
	if (asset == assets->camel_walk) {
		frame = int(Time::seconds() * PlayerWalkFPS) % asset->nframes;
		humpOff = frame % 2; // hack;
	} else {
		frame = Graphics::pingPong(int(Time::seconds() * PlayerIdleFPS), asset->nframes);
	}
	if (facingRight) {
		SpriteBatch::draw(asset, pixelPosition + vec(6, -4), frame);
		SpriteBatch::draw(assets->camel_hump, pixelPosition + vec(0, -10-humpOff));
	} else {
		SpriteBatch::drawFlippedH(asset, pixelPosition + vec(-6, -4), frame);
		SpriteBatch::drawFlippedH(assets->camel_hump, pixelPosition + vec(0, -10-humpOff));
	}
}

void Blindr::Player::drawIdle() {
	int frame = Graphics::pingPong(int(Time::seconds() * PlayerIdleFPS), assets->camel->nframes);
	vec2 pixelPosition = vec(PixelsPerMeter * body->GetPosition());
	SpriteBatch::draw(assets->camel, pixelPosition + vec(6, -4), frame);
	SpriteBatch::draw(assets->camel_hump, pixelPosition + vec(0, -10));
}




