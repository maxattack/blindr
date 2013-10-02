#pragma once
#include "LittlePolygon.h"
#include "Box2D.h"
#include "GameAssets.h"
#include "DebugDraw.h"

#define PixelsPerMeter   24
#define MetersPerPixel   (1.0f/PixelsPerMeter)
#define PlayerRunForce   12.0f
#define PlayerJumpForce  15.0f
#define PlayerWalkFPS    10
#define PlayerIdleFPS    12
#define PlayerHalfWidth  0.45f
#define PlayerHalfHeight 0.5f
#define TilemapOffsetX   3
#define ScrollMetersPerSecond 0.25f
#define GazerOffset      0.5f
#define TimeBetweenSpawns 3.75f
#define JuggleStrength  0.15f

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
		ctWall =       0x1<<1,
		ctPlayer =     0x1<<2,
		ctPlayerFoot = 0x1<<3,
		ctDebris =     0x1<<4,
		ctGazer =      0x1<<5
	};
	
	enum BodyUserDataType {
		btPlayer,
		btDebris
	};
	
	struct BodyUserdata {
		BodyUserDataType userType;

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
		
		void incrementGroundCount() { ++groundCount; }
		void decrementGroundCount() { if (groundCount) { --groundCount; } }
		bool grounded();

		void jump();
		
		void preTick();
		void postTick();
		void draw();
		
		void drawIdle();
		
		bool isHitbox(b2Fixture *fix);

	};
	
	class Debris : public BodyUserdata {
		b2Body *body;
		b2Vec2 juggleNormal;
		float lastJuggleTime;
		int juggleCount;
		bool inJuggleList;
		
	public:
		Debris(World *world);
		~Debris();
		
		bool shouldCull();
		b2Body* getBody() { return body; }
		
		void draw();
		bool hasBeenJuggled() const { return juggleCount > 0; }
		
		// juggles are buffered in the contact listener...
		bool didGetJuggled(b2Vec2 normal);
		// ...and then applied after the physics
		void applyJuggle();
		
		Debris *juggleNext;
		Debris *next;
		Debris *prev;
	};
	
	
	
	
	class Gazer {
		b2Body *body;
		vec2 irisOffset;
		float spotAngle;
	
	public:
		Gazer(World *world);
		
		b2Body* getBody() { return body; }
		
		void preTick();
		void postTick();
		void draw(float spotAmount=1.0f);
		void drawIntro(float introAmount);
		
		Debris* findCollidingDebris();
		
		void fillShadow();
	};




	class World : public b2ContactListener {
		b2World sim;
		Player *player;
		Gazer *gazer;
		b2Body *ground;
		Debris* headDebris;
		float scrollMeters;
		
		DebugDraw dbgDraw;
		bool doDebugDraw;
		float timeToNextSpawn;
		
		Debris *juggleHead;
		bool didHit;
		
	public:
		World();
		~World();

		b2World* getSim() { return &sim; }
		b2Vec2 screenSize() { return bvec(Graphics::canvasSize() * MetersPerPixel); }
		Player* getPlayer() { return player; }
		Gazer* getGazer() { return gazer; }
		float getScrollMeters() const { return scrollMeters; }
		
		void run();

		void BeginContact(b2Contact *contact);
		void EndContact(b2Contact *contact);
		void PreSolve(b2Contact *contact, const b2Manifold *oldManifold);
		void PostSolve(b2Contact *contact, const b2ContactImpulse *impulse);
	
	private:
		void titleScene();
		void introCutscene();
		bool introYield();
		
		void handleEvents();
		void handleContacts();
		void handleGazerHit();
		void createWalls();
		void createFloor(b2Vec2 p0, b2Vec2 p1);
		
		void onPlayerCollisionPreSolve(b2Fixture *fixture, b2Contact *contact);
		void onPlayerCollisionPostSolve(b2Fixture *fixture, b2Contact *contact);
		
		void checkForCloudPlatform(b2Fixture *fixture, b2Contact *contact);
		void checkForJuggle(b2Fixture *fixture, b2Contact *contact);
		
		void initializeGeometry(b2Vec2 *outPlayerPosition);
		
		void drawTilemap();
	};
	
	//---------------------------------------------------------------------------------------
	// TOP-LEVEL
	//---------------------------------------------------------------------------------------

	extern GameAssets *assets;
	extern World * world;
	
	void init();
}
