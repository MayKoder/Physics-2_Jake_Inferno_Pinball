#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModulePhysics.h"
#include "math.h"
#include"Animation.h"

// TODO 1
#include"Box2D/Box2D/Box2D.h"
#ifdef _DEBUG
	#pragma comment( lib, "Box2D/libx86/Box2D_Debug_lib/Box2D.lib" )

#else

	#pragma comment( lib, "Box2D/libx86/Box2D_Release_lib/Box2D.lib" )

#endif // _DEBUG


ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	world = NULL;
	mouse_joint = NULL;
}

// Destructor
ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Start()
{
	LOG("Creating Physics 2D environment");

	if (!world)
	{
		world = new b2World(b2Vec2(0.0f, 10.0f));
		world->SetContactListener(this);
	}

	b2BodyDef bd;
	ground = world->CreateBody(&bd);


	return true;
}

// 
update_status ModulePhysics::PreUpdate()
{
	// TODO 3: Update the simulation ("step" the world)
	for (int i = 0; i < (int)converter_list.count(); i++)
	{
		if (converter_list[i]->body->GetType() == b2BodyType::b2_dynamicBody) 
		{
			converter_list[i]->body->SetTransform(converter_list[i + 1]->body->GetPosition(), 0);
			converter_list.del(converter_list.At(i + 1));
			converter_list.del(converter_list.At(i));
		}
		else
		{
			converter_list[i]->body->SetActive(!converter_list[i]->body->IsActive());
			converter_list.del(converter_list.At(i));
		}
	}
	world->Step(timeStep, velocityIterations, positionIterations);

	for (b2Contact* c = world->GetContactList(); c; c = c->GetNext())
	{
		if (/*c->GetFixtureA()->IsSensor() && */c->IsTouching())
		{
			PhysBody* pb1 = (PhysBody*)c->GetFixtureA()->GetBody()->GetUserData();
			PhysBody* pb2 = (PhysBody*)c->GetFixtureB()->GetBody()->GetUserData();
			if (pb1 && pb2 && pb1->listener)
				pb1->listener->OnCollision(pb1, pb2);
		}
	}

	return UPDATE_CONTINUE;
}

// 
update_status ModulePhysics::PostUpdate()
{
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		App->input->debug = !App->input->debug;

	// Bonus code: this will iterate all objects in the world and draw the circles
	// You need to provide your own macro to translate meters to pixels
	if (App->input->debug) 
	{
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
			App->renderer->DrawLine(METERS_TO_PIXELS(mouse_joint->GetAnchorB().x), METERS_TO_PIXELS(mouse_joint->GetAnchorB().y), METERS_TO_PIXELS(mouse_joint->GetTarget().x)
				, METERS_TO_PIXELS(mouse_joint->GetTarget().y), 255, 0, 0, 255);
		}

	}

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
	for (int i = 0;  i < (int)world_body_list.count();  i++)
	{
		DestroyBody(world_body_list[i]->body);
		delete world_body_list[i];
	}

	// Delete the whole physics world!
	world_body_list.clear();

	for (int i = 0; i < (int)joint_body_list.count(); i++)
	{
		world->DestroyBody(joint_body_list[i]);
	}
	joint_body_list.clear();

	converter_list.clear();

	delete world;

	return true;
}

PhysBody* ModulePhysics::Create_Circle(int _x, int _y, float meter_radius, b2BodyType type, float density, SDL_Rect sec, int hit_score, int sound, Animation* hit_animation)
{
	b2BodyDef body;
	body.type = type;
	body.position.Set(PIXELS_TO_METERS(_x), PIXELS_TO_METERS(_y));
	body.bullet = true;

	b2Body* b = world->CreateBody(&body);

	b2CircleShape shape;
	shape.m_radius = meter_radius;


	b2FixtureDef fixture;
	fixture.density = density;
	fixture.shape = &shape;
	fixture.restitution = .5f;

	b->CreateFixture(&fixture);

	PhysBody* bdy = new PhysBody();
	bdy->body = b;
	if(sec.h != 0)
		bdy->spriteSheet = 0;
	bdy->soundOnHit = sound;

	//Animation
	bdy->idle.PushBack(sec);
	bdy->current_animation = bdy->idle;
	if (hit_animation)
		bdy->hit = *hit_animation;
	bdy->idle.speed = 0;

	bdy->scoreOnHit = hit_score;
	bdy->listener = App->main_level;

	b->SetUserData(bdy);

	return bdy;

}
PhysBody* ModulePhysics::Create_Poly(float x, float y, int points[], int count, b2Vec2 half_Array[], SDL_Rect sec, b2BodyType type, SDL_RendererFlip flip, b2Vec2 offset, float density, int hit_score, int sound, Animation* hit_animation)
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
		body.type = type;
		body.position.Set(PIXELS_TO_METERS(x), PIXELS_TO_METERS(y));

		b2Body* b = world->CreateBody(&body);

		PhysBody* bdy = new PhysBody();
		bdy->body = b;
		if (sec.h != 0)
			bdy->spriteSheet = 0;
		bdy->flip = flip;

		//Animation
		bdy->idle.PushBack(sec);
		bdy->current_animation = bdy->idle;
		bdy->idle.speed = 0;
		bdy->scoreOnHit = hit_score;
		if (hit_animation)
			bdy->hit = *hit_animation;
		bdy->offset = offset;
		bdy->soundOnHit = sound;
		bdy->needs_Center = false;

		b2PolygonShape shape;
		shape.Set(half_Array, count / 2);


		b2FixtureDef fixture;
		fixture.density = density;
		fixture.shape = &shape;

		bdy->body->CreateFixture(&fixture);
		b->SetUserData(bdy);

		return bdy;
	}

	return nullptr;

}
PhysBody* ModulePhysics::Create_Rectangle(SDL_Rect size, int type, float density, SDL_Rect sec, float angle, int hit_score, int sound, Animation* hit_animation)
{
	b2BodyDef body;
	body.type = (b2BodyType)type;
	body.position.Set(PIXELS_TO_METERS(size.x), PIXELS_TO_METERS(size.y));
	body.angle = angle * DEGTORAD;
	//body.active = false;

	b2Body* b = world->CreateBody(&body);

	b2PolygonShape shape;
	shape.SetAsBox(PIXELS_TO_METERS(size.w), PIXELS_TO_METERS(size.h));

	b2FixtureDef fixture;
	fixture.density = density;
	fixture.shape = &shape;
	b->CreateFixture(&fixture);

	PhysBody* bdy = new PhysBody();
	bdy->body = b;
	if (sec.h != 0)
		bdy->spriteSheet = 0;
	bdy->soundOnHit = sound;

	//Animation
	bdy->idle.PushBack(sec);
	bdy->current_animation = bdy->idle;
	if(hit_animation)
		bdy->hit = *hit_animation;
	bdy->idle.speed = 0;

	bdy->scoreOnHit = hit_score;
	b->SetUserData(bdy);

	return bdy;
}
PhysBody* ModulePhysics::Create_Chain(float x, float y, int points[], int count, b2Vec2 half_Array[], SDL_Rect sec, SDL_RendererFlip flip)
{

	int posOnH = 0;
	for (int i = 0; i < count; i += 2)
	{
		half_Array[posOnH].x = PIXELS_TO_METERS(points[i]);
		half_Array[posOnH].y = PIXELS_TO_METERS(points[i + 1]);
		posOnH++;
	}

	b2BodyDef body;
	body.type = b2BodyType::b2_staticBody;
	body.position.Set(PIXELS_TO_METERS(x), PIXELS_TO_METERS(y));
	
	b2Body* b = world->CreateBody(&body);

	b2ChainShape shape;
	shape.CreateLoop(half_Array, count / 2);

	b2FixtureDef fixture;
	fixture.shape = &shape;
	b->CreateFixture(&fixture);

	PhysBody* bdy = new PhysBody();
	bdy->body = b;
	if (sec.h != 0)
		bdy->spriteSheet = 0;

	//Animation
	bdy->idle.PushBack(sec);
	bdy->current_animation = bdy->idle;
	bdy->idle.speed = 0;


	bdy->flip = flip;
	bdy->needs_Center = false;
		
	b->SetUserData(bdy);

	return bdy;
}
b2RevoluteJoint* ModulePhysics::Create_Revolute_Joint(b2Body* body, float angle, int x, int y)
{

	b2RevoluteJointDef def;

	b2BodyDef pin;
	pin.position.x = PIXELS_TO_METERS(x);
	pin.position.y = PIXELS_TO_METERS(y);
	body->SetBullet(true);
	b2Body* deleteBd = world->CreateBody(&pin);
	joint_body_list.add(deleteBd);

	def.Initialize(deleteBd, body, deleteBd->GetWorldCenter());
	def.collideConnected = false;
	def.enableLimit = true;
	//def.enableMotor = true;
	if (angle < 0) 
	{
		def.lowerAngle = angle * DEGTORAD;
	}
	else
	{
		def.upperAngle = angle * DEGTORAD;
	}

	b2RevoluteJoint* revolute_joint_pointer = (b2RevoluteJoint*)world->CreateJoint(&def);

	return revolute_joint_pointer;


}

PhysBody* ModulePhysics::Create_Rectangle_Sensor(SDL_Rect rectangle, float rotation, SDL_Rect sec, SDL_RendererFlip flip, Animation* hit_animation, int hit_score)
{
	b2BodyDef body;
	body.type = b2BodyType::b2_staticBody;
	body.position.Set(PIXELS_TO_METERS(rectangle.x), PIXELS_TO_METERS(rectangle.y));
	body.angle = rotation * DEGTORAD;

	b2Body* b = world->CreateBody(&body);

	b2PolygonShape shape;
	shape.SetAsBox(PIXELS_TO_METERS(rectangle.w), PIXELS_TO_METERS(rectangle.h));

	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.isSensor = true;

	b->CreateFixture(&fixture);

	PhysBody* bdy = new PhysBody();
	bdy->body = b;
	if (sec.h != 0) 
	{
		bdy->scoreOnHit = hit_score;
		bdy->spriteSheet = 0;
		bdy->idle.PushBack(sec);
		if (hit_animation)
			bdy->hit = *hit_animation;
		bdy->current_animation = bdy->idle;
		bdy->idle.speed = 0;
	}
	bdy->soundOnHit = App->main_level->lateral_spring_sound;


	b->SetUserData(bdy);

	bdy->flip = flip;

	return bdy;
}
PhysBody* ModulePhysics::Create_Circle_Sensor(int _x, int _y, float meter_radius, b2BodyType type, float density,  SDL_Rect sec, int hit_score, int sound, Animation* hit_animation)
{
	b2BodyDef body;
	body.type = type;
	body.position.Set(PIXELS_TO_METERS(_x), PIXELS_TO_METERS(_y));

	b2Body* b = world->CreateBody(&body);

	b2CircleShape shape;
	shape.m_radius = meter_radius;


	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.isSensor = true;

	b->CreateFixture(&fixture);

	PhysBody* bdy = new PhysBody();
	bdy->body = b;
	if (sec.h != 0)
		bdy->spriteSheet = 0;
	bdy->soundOnHit = sound;

	//Animation
	bdy->idle.PushBack(sec);
	if (hit_animation)
		bdy->hit = *hit_animation;
	bdy->current_animation = bdy->idle;
	bdy->idle.speed = 0;


	bdy->scoreOnHit = hit_score;
	bdy->listener = App->main_level;

	b->SetUserData(bdy);

	return bdy;
}

//Body class definition
PhysBody::~PhysBody()
{
}

b2Vec2 PhysBody::GetPositionMeters() {
	return body->GetPosition();
}
int PhysBody::GetPositionPixels_X() 
{
	int r = METERS_TO_PIXELS(body->GetPosition().x);
	return r;
}
int PhysBody::GetPositionPixels_Y() 
{
	int r = METERS_TO_PIXELS(body->GetPosition().y);
	return r;
}

float PhysBody::GetRotation() {
	return (body->GetAngle() * RADTODEG);
}

bool ModulePhysics::MoveObjectSmooth(b2Vec2* position, b2Vec2 target_point, float32 speed) 
{

	bool ret = false;


	if (target_point.y >= position->y) 
	{
		if (position->y + speed >= target_point.y) 
		{
			position->y = target_point.y;
			ret = true;
		}
		else
		{
			position->y += speed;
		}
	}
	else if(target_point.y <= position->y)
	{
		if (position->y - speed <= target_point.y) 
		{
			position->y = target_point.y;
			ret = true;
		}
		else
		{
			position->y -= speed;
		}
	}

	return ret;

}

void ModulePhysics::BeginContact(b2Contact* contact)
{
	PhysBody* physA = (PhysBody*)contact->GetFixtureA()->GetBody()->GetUserData();
	PhysBody* physB = (PhysBody*)contact->GetFixtureB()->GetBody()->GetUserData();

	if (physA && physA->listener != NULL)
		physA->listener->OnCollision(physA, physB);

	if (physB && physB->listener != NULL)
		physB->listener->OnCollision(physB, physA);
}