#pragma once
#include "Module.h"
#include "Globals.h"

class b2World;
enum b2BodyType;

class ModulePhysics : public Module
{
public:
	ModulePhysics(Application* app, bool start_enabled = true);
	~ModulePhysics();

	void Create_Circle(float meter_x, float meter_y, float meter_radius, b2BodyType type, float density);
	void Create_Edge(float meter_x, float meter_y, float meter_x_2, float meter_y_2, b2BodyType type, float density);

	bool Start();
	update_status PreUpdate();
	update_status PostUpdate();
	bool CleanUp();

	float timeStep = 1.0f / 60.f;
	int velocityIterations = 10;
	int positionIterations = 8;

private:

	b2World *world = nullptr;
	bool debug;
};