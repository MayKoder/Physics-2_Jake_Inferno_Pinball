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

BodyClass ModulePhysics::Create_Circle(int _x, int _y, float meter_radius, int type, float density, int sheet, SDL_Rect sec)
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
	fixture.density = density;
	fixture.shape = &shape;

	bdy.body->CreateFixture(&fixture);

	return bdy; 

}

BodyClass ModulePhysics::CreateChain(float x, float y, int points[], int count, b2Vec2 half_Array[], int sheet, SDL_Rect sec, int isDynamic)
{

	int posOnH = 0;
	for (int i = 0; i < count; i += 2)
	{
		half_Array[posOnH].x = PIXELS_TO_METERS(points[i]);
		half_Array[posOnH].y = PIXELS_TO_METERS(points[i + 1]);
		posOnH++;
	}

	b2BodyDef body;
	if (isDynamic == 0) {
		body.type = b2_kinematicBody;
	}
	else
	{
		body.type = b2_dynamicBody;
	}
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
	fixture.density = 1.f;
	bdy.body->CreateFixture(&fixture);

	return bdy;
}

bool ModulePhysics::Start()
{
	LOG("Creating Physics 2D environment");

	if(!world)
		world = new b2World(b2Vec2(0.0f, 10.0f));


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

void ModulePhysics::DestroyBody(b2Body* bodyToDestroy) 
{
	world->DestroyBody(bodyToDestroy);
}

// Called before quitting
bool ModulePhysics::CleanUp()
{
	LOG("Destroying physics world");
	//Clear world body list
	for (int i = 0;  i < world_body_list.count();  i++)
	{
		DestroyBody(world_body_list[i].body);
	}

	// Delete the whole physics world!
	world_body_list.clear();
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

float BodyClass::GetRotation() {
	return (body->GetAngle() * RADTODEG);
}