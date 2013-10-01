#pragma once
#include "LittlePolygon.h"
#include "Box2D.h"
#include "GameAssets.h"
#include "DebugDraw.h"

#define PixelsPerMeter  24
#define MetersPerPixel  (1.0f/24.0f)
#define PlayerRunForce  10.0f
#define PlayerJumpForce 10.0f
#define PlayerIdleFPS   12

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
		
		b2Fixture* getFoot() { return foot; }
		void incrementGroundCount() { ++groundCount; }
		void decrementGroundCount() { if (groundCount) { --groundCount; } }
		bool grounded() { return groundCount > 0; }

		void jump();
		
		void preTick();
		void postTick();
		void draw();

	};

	class World : public b2ContactListener {
		b2World sim;
		Player player;
		DebugDraw dbgDraw;

	public:
		World();

		b2World* getSim() { return &sim; }
		vec2 screenSize() { return Graphics::canvasSize() * MetersPerPixel; }
		Player* getPlayer() { return &player; }
		
		void run();

		void BeginContact(b2Contact *contact);
		void EndContact(b2Contact *contact);
		void PreSolve(b2Contact *contact, const b2Manifold *oldManifold);
		void PostSolve(b2Contact *contact, const b2ContactImpulse *impulse);
		
	};


	extern GameAssets *assets;
	extern World * world;
	
	void init();
}
