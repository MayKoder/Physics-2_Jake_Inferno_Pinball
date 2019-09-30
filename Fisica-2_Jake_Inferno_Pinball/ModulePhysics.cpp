#include "Globals.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "math.h"
#include<time.h>

// TODO 1
#include"Box2D/Box2D/Box2D.h"
#ifdef _DEBUG
	#pragma comment( lib, "Box2D/libx86/Debug/Box2D.lib" )

#else

	#pragma comment( lib, "Box2D/libx86/Release/Box2D.lib" )

#endif // _DEBUG


ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	debug = true;
}

// Destructor
ModulePhysics::~ModulePhysics()
{
}

void ModulePhysics::Create_Circle(float meter_x, float meter_y, float meter_radius, b2BodyType type, float density)
{
	b2BodyDef groundBodyDef;
	groundBodyDef.type = type;
	groundBodyDef.position.Set(meter_x, meter_y);
	b2Body* groundBody = world->CreateBody(&groundBodyDef);

	b2CircleShape groundCircle;
	groundCircle.m_radius = meter_radius;
	groundBody->CreateFixture(&groundCircle, density);
}

void ModulePhysics::Create_Edge(float meter_x, float meter_y, float meter_x_2, float meter_y_2, b2BodyType type, float density)
{
	b2BodyDef groundBodyDef;
	groundBodyDef.type = type;
	b2Vec2 v1(meter_x, meter_y);
	b2Vec2 v2(meter_x_2, meter_y_2);
	b2Body* groundBody = world->CreateBody(&groundBodyDef);

	b2EdgeShape edgeShape;
	edgeShape.Set(v1, v2);
	groundBody->CreateFixture(&edgeShape, density);
}

bool ModulePhysics::Start()
{
	LOG("Creating Physics 2D environment");
	srand(time(NULL));
	// TODO 2: Create a private variable for the world
	// - You need to send it a default gravity
	// - You need init the world in the constructor
	// - Remember to destroy the world after using it
	if(!world)
		world = new b2World(b2Vec2(0.0f, 10.0f));


	//// TODO 4: Create a a big static circle as "ground"
	//Create_Circle(PIXELS_TO_METERS(SCREEN_WIDTH / 2.f), PIXELS_TO_METERS(SCREEN_HEIGHT / 2.f), 2.5f, b2_staticBody, 0.0f);


	////Extra TODO create ground
	//Create_Edge(0.0f, (float)PIXELS_TO_METERS(SCREEN_HEIGHT) - 0.7f, (float)PIXELS_TO_METERS(SCREEN_WIDTH), (float)PIXELS_TO_METERS(SCREEN_HEIGHT) - 0.7f, b2_staticBody, 0.0f);


	return true;
}

// 
update_status ModulePhysics::PreUpdate()
{
	// TODO 3: Update the simulation ("step" the world)
	world->Step(timeStep, velocityIterations, positionIterations);


	return UPDATE_CONTINUE;
}

// 
update_status ModulePhysics::PostUpdate()
{
	// TODO 5: On space bar press, create a circle on mouse position
	// - You need to transform the position / radius
	//if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) 
	//{
	//	float radius = 0.05f + (float)(rand()) / ((float) (RAND_MAX / (0.3f - 0.05f)));
	//	//float radius = 0.1f;
	//	Create_Circle(PIXELS_TO_METERS((float)App->input->GetMouseX()), PIXELS_TO_METERS((float)App->input->GetMouseY()), radius, b2_dynamicBody, 0.0f);

	//}



	if(App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	if(!debug)
		return UPDATE_CONTINUE;

	// Bonus code: this will iterate all objects in the world and draw the circles
	// You need to provide your own macro to translate meters to pixels
	for(b2Body* b = world->GetBodyList(); b; b = b->GetNext())
	{
		for(b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
		{
			switch(f->GetType())
			{
				case b2Shape::e_circle:
				{
					b2CircleShape* shape = (b2CircleShape*)f->GetShape();
					b2Vec2 pos = f->GetBody()->GetPosition();
					App->renderer->DrawCircle(METERS_TO_PIXELS(pos.x), METERS_TO_PIXELS(pos.y), METERS_TO_PIXELS(shape->m_radius), 255, 255, 255);
				}
				break;

				// You will have to add more cases to draw boxes, edges, and polygons ...
				case b2Shape::e_edge:
				{
					b2EdgeShape* shape = (b2EdgeShape*)f->GetShape();
					App->renderer->DrawLine(METERS_TO_PIXELS(shape->m_vertex1.x), METERS_TO_PIXELS(shape->m_vertex1.y), METERS_TO_PIXELS(shape->m_vertex2.x), METERS_TO_PIXELS(shape->m_vertex2.y), 255, 255, 255);
				}
				break;



			}
		}
	}

	return UPDATE_CONTINUE;
}


// Called before quitting
bool ModulePhysics::CleanUp()
{
	LOG("Destroying physics world");

	// Delete the whole physics world!
	delete world;

	return true;
}
