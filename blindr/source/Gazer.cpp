#include "Blindr.h"
#include "Time.h"

Blindr::Gazer::Gazer(World *world) : irisOffset(vec(0,0)) {

	b2BodyDef bodyParams;
	bodyParams.type = b2_kinematicBody;
	bodyParams.position.x = 0.5f * world->screenSize().x;
	bodyParams.position.y = world->getScrollMeters() + GazerOffset;
	bodyParams.fixedRotation = true;
	body = world->getSim()->CreateBody(&bodyParams);
	
	b2CircleShape shapeParams;
	shapeParams.m_radius = 1;
	
	b2FixtureDef fixParams;
	fixParams.isSensor = true;
	fixParams.shape = &shapeParams;
	body->CreateFixture(&fixParams);
	
	b2Vec2 toPlayer = world->getPlayer()->getBody()->GetPosition() - body->GetPosition();
	irisOffset = vec(0.5f * toPlayer);
	spotAngle = irisOffset.radians();
	
}

void Blindr::Gazer::preTick() {
}

void Blindr::Gazer::postTick() {
	b2Vec2 p = body->GetPosition();
	p.y = world->getScrollMeters() + GazerOffset;
	body->SetTransform(p, 0);

	b2Vec2 toPlayer = world->getPlayer()->getBody()->GetPosition() - p;
	irisOffset = vec(toPlayer);
	if (irisOffset.norm() > 10 * 10) {
		irisOffset = 10 * irisOffset.normalized();
	}
}

		
void Blindr::Gazer::draw(float spotAmount) {
	vec2 pp = PixelsPerMeter * vec(body->GetPosition());
	SpriteBatch::draw(assets->gazer_body, pp);
	SpriteBatch::end();
	
	// draw the shadow areas
	{
		float da = spotAmount * 0.1f * M_PI;
		Graphics::ScopedTransform move(translationMatrix(pp));
		
		spotAngle = easeRadians(spotAngle, irisOffset.radians(), 0.1f, Time::deltaSeconds());
		
		Graphics::ScopedTransform rotate(attitudeMatrix( polar(1, spotAngle) ));
		Graphics::ScopedTransform moveLocal(translationMatrix(vec(-48, 0)));
		VectorGraphics::drawSemicircle(vec(0,0), rgba(0.01f, 0.01f, 0.1f, 0.6f), Graphics::canvasSize().magnitude(), da, 2.0f * M_PI - da, 64);
		if (spotAmount > 0.0001f) {
			VectorGraphics::drawSemicircle(vec(0,0), rgba(0.01f, 0.01f, 0.1f, 0.6f), 32, 2.0f * M_PI - da, 2.0f * M_PI + da, 16);
		}
	}
	
	SpriteBatch::begin(assets->Sprites);
	SpriteBatch::draw(assets->gazer_eyeball, pp);
	SpriteBatch::draw(assets->gazer_iris, pp + irisOffset);
}

void Blindr::Gazer::drawIntro(float introAmount) {
	float u = 1.0f - introAmount;
	
	vec2 pp = PixelsPerMeter * vec(body->GetPosition()) - vec(0, u * 100);
	SpriteBatch::draw(assets->gazer_body, pp);
	SpriteBatch::end();
	
	// draw the shadow areas
	{
		Graphics::ScopedTransform move(translationMatrix(pp));
		Graphics::ScopedTransform rotate(attitudeMatrix( polar(1, spotAngle) ));
		Graphics::ScopedTransform moveLocal(translationMatrix(vec(-48, 0)));
		VectorGraphics::fillScreen(rgba(0.01f, 0.01f, 0.1f, 0.6f));
	}
	
	SpriteBatch::begin(assets->Sprites);
	SpriteBatch::draw(assets->gazer_eyeball, pp);
	SpriteBatch::draw(assets->gazer_iris, pp + irisOffset);
	
}

void Blindr::Gazer::fillShadow() {
	VectorGraphics::fillScreen(rgba(0.01f, 0.01f, 0.1f, 0.6f));
}


