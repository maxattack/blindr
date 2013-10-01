#pragma once
#include "LittlePolygon.h"
#include "Box2D.h"
#include "GameAssets.h"
#include "DebugDraw.h"

#define PixelsPerMeter   24
#define MetersPerPixel   (1.0f/PixelsPerMeter)
#define PlayerRunForce   10.0f
#define PlayerJumpForce  13.5f
#define PlayerWalkFPS    10
#define PlayerIdleFPS    12
#define PlayerHalfWidth  0.45f
#define PlayerHalfHeight 0.5f
#define TilemapOffsetX   8
#define ScrollMetersPerSecond 0.25f

inline vec2 vec(b2Vec2 bv) { return vec(bv.x, bv.y); }
inline b2Vec2 bvec(vec2 v) { return b2Vec2(v.x, v.y); }

namespace Blindr {

	//---------------------------------------------------------------------------------------
	// BASE DECLS
	//---------------------------------------------------------------------------------------

	class World;
	class Player;
	class Debris;

	enum CollisionTypes {
		ctFloor =      0x1,
		ctPlayer =     0x1<<1,
		ctPlayerFoot = 0x1<<2,
		ctDebris =     0x1<<3,
		ctGazer =      0x1<<4
	};
	
	enum BodyUserDataType {
		btPlayer,
		btDebris
	};
	
	class BodyUserdata {
		BodyUserDataType userType;
	public:
		BodyUserdata(BodyUserDataType aType) : userType(aType) {}
		
		bool isPlayer() const { return userType == btPlayer; }
		bool isDebris() const { return userType == btDebris; }
	};
	
	struct FloorData {
		int tx, ty, tw;
	};
	
	struct BoxData {
		int tx, ty;
	};
	
	extern int kFloorCount;
	extern int kBoxCount;
	extern int kLevelW;
	extern int kLevelH;
	extern FloorData kFloors[];
	extern BoxData kBoxes[];
	
	//---------------------------------------------------------------------------------------
	// GAME OBJECTS
	//---------------------------------------------------------------------------------------

	class Player : public BodyUserdata {
		b2Body *body;
		b2Fixture *hitbox;
		b2Fixture *foot;
		int groundCount;
		bool facingRight;
		bool pressingLeft;
		bool pressingRight;

	public:
		Player(World *world, b2Vec2 playerPos);
		
		
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
	
	class Debris : public BodyUserdata {
		b2Body *body;
		
	public:
		Debris();
		Debris& init();
		Debris& release();
		
		void draw();
	};
	
	class Gazer {
		b2Body *body;
		vec2 irisOffset;
		float spotAngle;
	
	public:
		Gazer(World *world);
		
		void preTick();
		void postTick();
		void draw();
	};

	class World : public b2ContactListener {
		b2World sim;
		Player *player;
		Gazer *gazer;
		Pool<Debris, 64> debris;
		float scrollMeters;
		
		DebugDraw dbgDraw;
		bool doDebugDraw;
		
		
	public:
		World();

		b2World* getSim() { return &sim; }
		b2Vec2 screenSize() { return bvec(Graphics::canvasSize() * MetersPerPixel); }
		Player* getPlayer() { return player; }
		Gazer* getGazer() { return gazer; }
		
		void run();

		void BeginContact(b2Contact *contact);
		void EndContact(b2Contact *contact);
		void PreSolve(b2Contact *contact, const b2Manifold *oldManifold);
		void PostSolve(b2Contact *contact, const b2ContactImpulse *impulse);
	
	private:
		void handleEvents();
		b2Body *createFloor(b2Vec2 p0, b2Vec2 p1);
		void checkForCloudPlatform(b2Fixture *fixture, b2Contact *contact);
		
		void initializeGeometry(b2Vec2 *outPlayerPosition);
	};
	
	//---------------------------------------------------------------------------------------
	// TOP-LEVEL
	//---------------------------------------------------------------------------------------

	extern GameAssets *assets;
	extern World * world;
	
	void init();
}
