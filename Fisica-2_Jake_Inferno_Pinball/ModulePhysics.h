#pragma once
#include "Module.h"
#include "Globals.h"
#include"p2List_Extended.h"

class b2World;
enum b2BodyType;
class b2Body;
struct b2Vec2;
typedef float float32;
typedef int int32;

class ModulePhysics : public Module
{
public:
	ModulePhysics(Application* app, bool start_enabled = true);
	~ModulePhysics();

	void Create_Circle(float meter_x, float meter_y, float meter_radius, b2BodyType type, float density);
	void Create_Edge(float meter_x, float meter_y, float meter_x_2, float meter_y_2, b2BodyType type, float density);
	void CreateChain(float, float, int[], int, b2Vec2[]);

	bool Start();
	update_status PreUpdate();
	update_status PostUpdate();
	bool CleanUp();

	float32 timeStep = 1.0f / 60.f;
	int32 velocityIterations = 10;
	int32 positionIterations = 8;

public:
	p2List_Extended<BodyClass> *world_body_list;

private:

	b2World *world = nullptr;
	bool debug;
};

class BodyClass
{
public:
	BodyClass();
	~BodyClass();

	b2Body* body;


	b2Vec2 GetPositionPixels();
	b2Vec2 GetPositionMeters();


};
