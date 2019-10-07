#pragma once
#include "Module.h"
#include "Globals.h"
#include"p2List_Extended.h"

class b2World;
class b2Body;
struct b2Vec2;
typedef float float32;
typedef int int32;

class BodyClass
{
public:
	BodyClass();
	~BodyClass();

	b2Body* body;
	int spriteSheet;
	SDL_Rect section;
	bool needs_Center = true;
	int pivotX = 0, pivotY = 0;


	int GetPositionPixels_X();
	int GetPositionPixels_Y();

	float GetRotation();

	b2Vec2 GetPositionMeters();
};

class ModulePhysics : public Module
{
public:
	ModulePhysics(Application* app, bool start_enabled = true);
	~ModulePhysics();

	BodyClass Create_Circle(int _x, int _y, float meter_radius, int type, float density, int sheet, SDL_Rect sec);
	BodyClass CreateChain(float, float, int[], int, b2Vec2[], int sheet = -1, SDL_Rect sec = { 0, 0, 0, 0 }, int isDynamic = 0);

	bool Start();
	update_status PreUpdate();
	update_status PostUpdate();
	bool CleanUp();

	float32 timeStep = 1.0f / 60.f;
	int32 velocityIterations = 10;
	int32 positionIterations = 8;

public:
	p2List_Extended<BodyClass> world_body_list;
	void DestroyBody(b2Body*);

private:
	b2World *world = nullptr;
	bool debug;
};


