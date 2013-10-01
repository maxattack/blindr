#pragma once
#include "LittlePolygon.h"
#include "Box2D.h"
#include "GameAssets.h"
#include "DebugDraw.h"

#define PixelsPerMeter   24
#define MetersPerPixel   (1.0f/PixelsPerMeter)
#define PlayerRunForce   10.0f
#define PlayerJumpForce  13.0f
#define PlayerIdleFPS    12
#define PlayerHalfWidth  0.5f
#define PlayerHalfHeight 0.5f

inline vec2 vec(b2Vec2 bv) { return vec(bv.x, bv.y); }
inline b2Vec2 bvec(vec2 v) { return b2Vec2(v.x, v.y); }

namespace Blindr {

	class World;
	class Player;

	inline int pingPong(int i, int n) {
		i  = i % (2 * n - 2);
		if (i >= n) {
			return 2 * (n-1) - i + 1;
		} else {
			return i;
		}
	}

	enum CollisionTypes {
		ctFloor = 0x001,
		ctPlayer = 0x002,
		ctPlayerFoot = 0x004
	};

	class Player {
		b2Body *body;
		b2Fixture *hitbox;
		b2Fixture *foot;
		int groundCount;
		bool facingRight;
		bool pressingLeft;
		bool pressingRight;

	public:
		Player(World *world);
		
		
		void leftPressed() { pressingLeft = true; facingRight = false; }
		void leftReleased() { pressingLeft = false; }
		void rightPressed() { pressingRight = true; facingRight = true; }
		void rightReleased() { pressingRight = false; }
		
		b2Body* getBody() { return body; }
		b2Fixture* getFoot() { return foot; }
		b2Fixture* getHitbox() { return hitbox; }
		
		void incrementGroundCount() { ++groundCount; }
		void decrementGroundCount() { if (groundCount) { --groundCount; } }
		bool grounded();

		void jump();
		
		void preTick();
		void postTick();
		void draw();

	};

	class World : public b2ContactListener {
		b2World sim;
		Player player;
		DebugDraw dbgDraw;
		bool doDebugDraw;

	public:
		World();

		b2World* getSim() { return &sim; }
		b2Vec2 screenSize() { return bvec(Graphics::canvasSize() * MetersPerPixel); }
		Player* getPlayer() { return &player; }
		
		void run();

		void BeginContact(b2Contact *contact);
		void EndContact(b2Contact *contact);
		void PreSolve(b2Contact *contact, const b2Manifold *oldManifold);
		void PostSolve(b2Contact *contact, const b2ContactImpulse *impulse);
	
	private:
		void handleEvents();
		b2Body *createFloor(b2Vec2 p0, b2Vec2 p1);
		void checkForCloudPlatform(b2Fixture *fixture, b2Contact *contact);
	};


	extern GameAssets *assets;
	extern World * world;
	
	void init();
}
