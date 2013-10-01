#include "Blindr.h"
#include "Time.h"

Blindr::Gazer::Gazer(World *world) : irisOffset(vec(0,0)) {

//	b2BodyDef bodyParams;
//	bodyParams.position.x = 0.5f * world->screenSize().x;
//	bodyParams.position.y = 40 * MetersPerPixel;
//	bodyParams.fixedRotation = true;
//	body = world->getSim()->CreateBody(&bodyParams);
//	
//	b2CircleShape shapeParams;
//	shapeParams.m_radius = 1;
//	
//	b2FixtureDef fixParams;
//	fixParams.isSensor = true;
//	fixParams.shape = &shapeParams;
//	body->CreateFixture(&fixParams);
//	
//	b2Vec2 toPlayer = world->getPlayer()->getBody()->GetPosition() - body->GetPosition();
//	irisOffset = vec(0.5f * toPlayer);
//	spotAngle = irisOffset.radians();
	
}

void Blindr::Gazer::preTick() {
	
}

void Blindr::Gazer::postTick() {
	b2Vec2 toPlayer = world->getPlayer()->getBody()->GetPosition() - body->GetPosition();
	irisOffset = vec(0.5f * toPlayer);
}

		
void Blindr::Gazer::draw() {
	vec2 cs = Graphics::canvasSize();
	float mp = 0.5f * cs.x;
	SpriteBatch::draw(assets->gazer_body, vec(mp, 40));
	SpriteBatch::end();
	
	// draw the shadow areas
	{
		Graphics::ScopedTransform move(translationMatrix(vec(body->GetPosition()) * PixelsPerMeter));
		
		spotAngle = easeTowards(spotAngle, irisOffset.radians(), 0.1f, Time::deltaSeconds());
		
		Graphics::ScopedTransform rotate(attitudeMatrix( polar(1, spotAngle) ));
		Graphics::ScopedTransform moveLocal(translationMatrix(vec(-48, 0)));
		VectorGraphics::drawSemicircle(vec(0,0), rgba(0.01f, 0.01f, 0.1f, 0.6f), cs.magnitude(), 0.1f * M_PI, 1.9f * M_PI, 64);
		VectorGraphics::drawSemicircle(vec(0,0), rgba(0.01f, 0.01f, 0.1f, 0.6f), 32, 1.9f * M_PI, 2.1f * M_PI, 16);
	}
	
	SpriteBatch::begin(assets->Sprites);
	SpriteBatch::draw(assets->gazer_eyeball, vec(mp, 40));
	SpriteBatch::draw(assets->gazer_iris, vec(mp, 40) + irisOffset);
}

