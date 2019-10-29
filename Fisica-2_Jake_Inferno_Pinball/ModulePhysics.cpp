#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModulePhysics.h"
#include "math.h"

// TODO 1
#include"Box2D/Box2D/Box2D.h"
#ifdef _DEBUG
	#pragma comment( lib, "Box2D/libx86/Box2D_Debug_lib/Box2D.lib" )

#else

	#pragma comment( lib, "Box2D/libx86/Box2D_Release_lib/Box2D.lib" )

#endif // _DEBUG


ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	debug = true;
}

// Destructor
ModulePhysics::~ModulePhysics()
{
}

BodyClass ModulePhysics::Create_Circle(int _x, int _y, float meter_radius, int type, float density, int sheet, SDL_Rect sec, SDL_RendererFlip flip)
{
	b2BodyDef body;
	body.type = (b2BodyType)type;
	body.position.Set(PIXELS_TO_METERS(_x), PIXELS_TO_METERS(_y));

	BodyClass bdy;
	bdy.body = world->CreateBody(&body);
	bdy.spriteSheet = sheet;
	bdy.section = sec;
	bdy.flip = flip;

	b2CircleShape shape;
	shape.m_radius = meter_radius;


	b2FixtureDef fixture;
	fixture.density = density;
	fixture.shape = &shape;
	fixture.restitution = .5f;

	bdy.body->CreateFixture(&fixture);

	return bdy; 

}

BodyClass ModulePhysics::Create_Poly(float x, float y, int points[], int count, b2Vec2 half_Array[], int sheet, SDL_Rect sec, int type, SDL_RendererFlip flip, float density)
{


	if (count / 2 > 8) 
	{

		LOG("ERROR ON POLY CREATION, TOO MUCH POINTS");

	}
	else
	{
		int posOnH = 0;
		for (int i = 0; i < count; i += 2)
		{
			half_Array[posOnH].x = PIXELS_TO_METERS(points[i]);
			half_Array[posOnH].y = PIXELS_TO_METERS(points[i + 1]);
			posOnH++;
		}


		b2BodyDef body;
		body.type = (b2BodyType)type;
		body.position.Set(PIXELS_TO_METERS(x), PIXELS_TO_METERS(y));

		BodyClass bdy;
		bdy.body = world->CreateBody(&body);
		bdy.spriteSheet = sheet;
		bdy.section = sec;
		bdy.flip = flip;
		bdy.needs_Center = false;

		b2PolygonShape shape;
		shape.Set(half_Array, count / 2);


		b2FixtureDef fixture;
		fixture.density = density;
		fixture.shape = &shape;

		bdy.body->CreateFixture(&fixture);

		return bdy;
	}

}


BodyClass ModulePhysics::Create_Rectangle(SDL_Rect size, int type, float density, int sheet, SDL_Rect sec, SDL_RendererFlip flip) 
{
	b2BodyDef body;
	body.type = (b2BodyType)type;
	body.position.Set(PIXELS_TO_METERS(size.x), PIXELS_TO_METERS(size.y));

	BodyClass bdy;
	bdy.body = world->CreateBody(&body);
	bdy.spriteSheet = sheet;
	bdy.section = sec;
	bdy.flip = flip;

	b2PolygonShape shape;
	shape.SetAsBox(PIXELS_TO_METERS(size.w), PIXELS_TO_METERS(size.h));


	b2FixtureDef fixture;
	fixture.density = density;
	fixture.shape = &shape;

	bdy.body->CreateFixture(&fixture);

	return bdy;
}

BodyClass ModulePhysics::Create_Chain(float x, float y, int points[], int count, b2Vec2 half_Array[], int sheet, SDL_Rect sec, int isDynamic, SDL_RendererFlip flip)
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
	bdy.flip = flip;
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

	b2BodyDef bd;
	ground = world->CreateBody(&bd);


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


			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN && f->GetBody()->GetType() != b2BodyType::b2_kinematicBody)
			{
				if (f->GetBody()->GetFixtureList()->TestPoint({ PIXELS_TO_METERS(App->input->GetMouseX()),PIXELS_TO_METERS(App->input->GetMouseY()) }))
				{
					jointBody = f->GetBody();
					b2MouseJointDef def;
					def.bodyA = ground;
					def.bodyB = jointBody;
					def.target = { PIXELS_TO_METERS(App->input->GetMouseX()),PIXELS_TO_METERS(App->input->GetMouseY()) };
					def.dampingRatio = 0.5f;
					def.frequencyHz = 2.0f;
					def.maxForce = 100.0f * jointBody->GetMass();
					mouse_joint = (b2MouseJoint*)world->CreateJoint(&def);
				}
			}

		}
	}

	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && mouse_joint)
	{
		mouse_joint->SetTarget({ PIXELS_TO_METERS(App->input->GetMouseX()),PIXELS_TO_METERS(App->input->GetMouseY()) });
		App->renderer->DrawLine(METERS_TO_PIXELS(jointBody->GetPosition().x), METERS_TO_PIXELS(jointBody->GetPosition().y), METERS_TO_PIXELS(mouse_joint->GetTarget().x)
			, METERS_TO_PIXELS(mouse_joint->GetTarget().y), 255, 0, 0, 255);
	}




	// TODO 4: If the player releases the mouse button, destroy the joint
	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP && mouse_joint != nullptr)
	{
		world->DestroyJoint(mouse_joint);
		mouse_joint = nullptr;
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

b2RevoluteJoint* ModulePhysics::Create_Revolute_Joint(b2Fixture* ball, b2Fixture* body)
{

	b2RevoluteJointDef def;
	def.bodyA = ball->GetBody();
	def.bodyB = body->GetBody();
	def.collideConnected = true;
	//def.localAnchorB = ball->GetBody()->GetPosition();

	//def.localAnchorA = ball->GetBody()->GetPosition();
	//def.localAnchorB = ball->GetBody()->GetPosition();


	b2RevoluteJoint* revolute_joint_pointer = (b2RevoluteJoint*)world->CreateJoint(&def);


	return revolute_joint_pointer;


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