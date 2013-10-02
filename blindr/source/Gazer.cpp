#include "Blindr.h"
#include "Time.h"

Blindr::Gazer::Gazer(World *world) :
irisOffset(vec(0,0)),
blinkTime(-1.0f),
damage(0) {

	b2BodyDef bodyParams;
	bodyParams.type = b2_kinematicBody;
	bodyParams.position.x = 0.5f * world->screenSize().x;
	bodyParams.position.y = world->getScrollMeters() + GazerOffset;
	bodyParams.fixedRotation = true;
	body = world->getSim()->CreateBody(&bodyParams);
	
	b2CircleShape shapeParams;
	shapeParams.m_radius = 0.6f;
	
	b2FixtureDef fixParams;
	fixParams.isSensor = true;
	fixParams.shape = &shapeParams;
	fixParams.filter.categoryBits = ctGazer;
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

inline int gazerFrame() {
	return Graphics::pingPong(int(Time::seconds() * GazerFPS), Blindr::assets->gazer_body->nframes);
}
		
void Blindr::Gazer::drawRaw(float spotAmount, bool modAlpha) {
	vec2 pp = PixelsPerMeter * vec(body->GetPosition());
	
	SpriteBatch::draw(assets->gazer_body, pp, gazerFrame());
	
	SpriteBatch::end();
	
	float alpha = 0.6f;
	if (modAlpha) { alpha += 0.35f  * (1-spotAmount); }
	
	// draw the shadow areas
	{
		float da = spotAmount * 0.1f * M_PI;
		Graphics::ScopedTransform move(translationMatrix(pp));
		
		spotAngle = easeRadians(spotAngle, irisOffset.radians(), 0.1f, Time::deltaSeconds());
		
		Graphics::ScopedTransform rotate(attitudeMatrix( polar(1, spotAngle) ));
		Graphics::ScopedTransform moveLocal(translationMatrix(vec(-48, 0)));
		VectorGraphics::drawSemicircle(vec(0,0), rgba(0.01f, 0.01f, 0.1f, alpha), Graphics::canvasSize().magnitude(), da, 2.0f * M_PI - da, 64);
		if (spotAmount > 0.0001f) {
			VectorGraphics::drawSemicircle(vec(0,0), rgba(0.01f, 0.01f, 0.1f, alpha), 32, 2.0f * M_PI - da, 2.0f * M_PI + da, 16);
		}
	}
	
	SpriteBatch::begin(assets->Sprites);
	SpriteBatch::draw(assets->gazer_eyeball, pp);
	SpriteBatch::draw(assets->gazer_iris, pp + irisOffset);
}

void Blindr::Gazer::drawIntro(float introAmount) {
	float u = 1.0f - introAmount;
	
	vec2 pp = PixelsPerMeter * vec(body->GetPosition()) - vec(0, u * 100);
	
	SpriteBatch::draw(assets->gazer_body, pp, gazerFrame());
	
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

Blindr::Debris* Blindr::Gazer::findCollidingDebris() {
	b2ContactEdge *edge = body->GetContactList();
	while(edge) {
		if (edge->other->GetUserData()) {
			BodyUserdata *buddy = (BodyUserdata*) edge->other->GetUserData();
			if (buddy->isDebris()) {
				return (Debris*) buddy;
			}
		}
		
		
		edge = edge->next;
	}

	return 0;
}

void Blindr::Gazer::blink() {
	if (blinkTime < 0) {
		blinkTime = 0.666f;
	}
}

void Blindr::Gazer::draw() {
	if (blinkTime > 0) {
		float u = 1 - (blinkTime / 0.666f);
		blinkTime -= Time::deltaSeconds();
		drawRaw(1 - parabola(u), true);
		int lidFrame = int(3 * u) % 2;
		SpriteBatch::draw(assets->gazer_eyelid, PixelsPerMeter * vec(body->GetPosition()), lidFrame);
	} else {
		drawRaw();
	}
		
}

bool Blindr::Gazer::drawOutro() {
	LOG_FLOAT(blinkTime);
	if (blinkTime > 0.333f) {
		float u = 1 - (blinkTime / 0.666f);
		blinkTime -= Time::deltaSeconds();
		drawRaw(1 - parabola(u), true);
		int lidFrame = int(3 * u) % 2;
		SpriteBatch::draw(assets->gazer_eyelid, PixelsPerMeter * vec(body->GetPosition()), lidFrame);
		return false;
	} else {
		drawRaw(0, true);
		SpriteBatch::draw(assets->gazer_eyelid, PixelsPerMeter * vec(body->GetPosition()), 1);
		return true;
	}
}


