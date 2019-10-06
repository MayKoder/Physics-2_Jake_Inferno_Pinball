#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModulePhysics.h"
#include "math.h"

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

BodyClass ModulePhysics::Create_Circle(int _x, int _y, float meter_radius, int type, float density, int sheet = -1, SDL_Rect sec = {0, 0, 0, 0})
{
	b2BodyDef body;
	body.type = (b2BodyType)type;
	body.position.Set(PIXELS_TO_METERS(_x), PIXELS_TO_METERS(_y));

	BodyClass bdy;
	bdy.body = world->CreateBody(&body);
	bdy.spriteSheet = sheet;
	bdy.section = sec;

	b2CircleShape shape;
	shape.m_radius = meter_radius;


	b2FixtureDef fixture;
	fixture.shape = &shape;

	bdy.body->CreateFixture(&fixture);

	return bdy; 

}

BodyClass ModulePhysics::CreateChain(float x, float y, int points[], int count, b2Vec2 half_Array[], int sheet = -1, SDL_Rect sec = { 0, 0, 0, 0 })
{

	int posOnH = 0;
	for (int i = 0; i < count; i += 2)
	{
		half_Array[posOnH].x = PIXELS_TO_METERS(points[i]);
		half_Array[posOnH].y = PIXELS_TO_METERS(points[i + 1]);
		posOnH++;
	}

	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXELS_TO_METERS(x), PIXELS_TO_METERS(y));

	BodyClass bdy;
	bdy.body = world->CreateBody(&body);
	bdy.spriteSheet = sheet;
	bdy.section = sec;
	bdy.needs_Center = false;

	b2ChainShape shape;
	shape.CreateLoop(half_Array, count / 2);

	b2FixtureDef fixture;
	fixture.shape = &shape;

	bdy.body->CreateFixture(&fixture);

	return bdy;
}

bool ModulePhysics::Start()
{
	LOG("Creating Physics 2D environment");

	if(!world)
		world = new b2World(b2Vec2(0.0f, 10.0f));

	// Pivot 0, 0
	int points[190] = {
		2, 2,
		328, 2,
		328, 1003,
		323, 1003,
		324, 651,
		322, 639,
		319, 626,
		315, 614,
		310, 602,
		305, 592,
		299, 582,
		291, 571,
		282, 560,
		273, 552,
		264, 544,
		254, 537,
		244, 531,
		232, 525,
		220, 521,
		214, 519,
		210, 514,
		210, 508,
		213, 502,
		221, 498,
		232, 494,
		243, 490,
		257, 484,
		273, 478,
		286, 472,
		293, 469,
		298, 463,
		299, 457,
		299, 362,
		275, 339,
		275, 278,
		305, 233,
		305, 148,
		304, 133,
		300, 116,
		293, 97,
		285, 82,
		277, 70,
		264, 55,
		249, 41,
		235, 32,
		221, 25,
		205, 19,
		193, 15,
		180, 13,
		160, 12,
		142, 13,
		124, 17,
		109, 22,
		95, 28,
		82, 36,
		71, 44,
		57, 57,
		46, 70,
		37, 85,
		29, 100,
		24, 114,
		21, 128,
		19, 143,
		18, 165,
		18, 233,
		36, 277,
		36, 340,
		12, 364,
		12, 460,
		15, 465,
		19, 469,
		27, 473,
		97, 501,
		101, 506,
		101, 512,
		99, 517,
		92, 521,
		81, 527,
		71, 534,
		62, 542,
		54, 551,
		46, 560,
		39, 571,
		32, 583,
		26, 597,
		23, 608,
		20, 623,
		18, 638,
		18, 740,
		46, 775,
		46, 835,
		12, 866,
		12, 1003,
		3, 1003,
		3, 768
	};
	b2Vec2 half_Array[(sizeof(points) / sizeof(int)) /2];
	world_body_list.add(CreateChain(8, -(1000 - (SCREEN_HEIGHT - 12)), *&points, (sizeof(points) / sizeof(int)), *&half_Array));


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
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	if (!debug)
		return UPDATE_CONTINUE;
	// Bonus code: this will iterate all objects in the world and draw the circles
	// You need to provide your own macro to translate meters to pixels
	for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
	{
		for (b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
		{
			switch (f->GetType())
			{
				// Draw circles ------------------------------------------------
			case b2Shape::e_circle:
			{
				b2CircleShape* shape = (b2CircleShape*)f->GetShape();
				b2Vec2 pos = f->GetBody()->GetPosition();
				App->renderer->DrawCircle(METERS_TO_PIXELS(pos.x), METERS_TO_PIXELS(pos.y), METERS_TO_PIXELS(shape->m_radius), 255, 255, 255);
			}
			break;

			// Draw polygons ------------------------------------------------
			case b2Shape::e_polygon:
			{
				b2PolygonShape* polygonShape = (b2PolygonShape*)f->GetShape();
				int32 count = polygonShape->GetVertexCount();
				b2Vec2 prev, v;

				for (int32 i = 0; i < count; ++i)
				{
					v = b->GetWorldPoint(polygonShape->GetVertex(i));
					if (i > 0)
						App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);

					prev = v;
				}

				v = b->GetWorldPoint(polygonShape->GetVertex(0));
				App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);
			}
			break;

			// Draw chains contour -------------------------------------------
			case b2Shape::e_chain:
			{
				b2ChainShape* shape = (b2ChainShape*)f->GetShape();
				b2Vec2 prev, v;

				for (int32 i = 0; i < shape->m_count; ++i)
				{
					v = b->GetWorldPoint(shape->m_vertices[i]);
					if (i > 0)
						App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
					prev = v;
				}

				v = b->GetWorldPoint(shape->m_vertices[0]);
				App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
			}
			break;

			// Draw a single segment(edge) ----------------------------------
			case b2Shape::e_edge:
			{
				b2EdgeShape* shape = (b2EdgeShape*)f->GetShape();
				b2Vec2 v1, v2;

				v1 = b->GetWorldPoint(shape->m_vertex0);
				v1 = b->GetWorldPoint(shape->m_vertex1);
				App->renderer->DrawLine(METERS_TO_PIXELS(v1.x), METERS_TO_PIXELS(v1.y), METERS_TO_PIXELS(v2.x), METERS_TO_PIXELS(v2.y), 100, 100, 255);
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


//Body class definition
BodyClass::BodyClass()
{
	body = nullptr;
}

BodyClass::~BodyClass()
{
}

b2Vec2 BodyClass::GetPositionMeters() {
	return body->GetPosition();
}
int BodyClass::GetPositionPixels_X() 
{
	int r = METERS_TO_PIXELS(body->GetPosition().x);
	return r;
}
int BodyClass::GetPositionPixels_Y() 
{
	int r = METERS_TO_PIXELS(body->GetPosition().y);
	return r;
}