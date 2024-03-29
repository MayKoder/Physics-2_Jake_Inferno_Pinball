#ifndef __ModulePhysics_H__
#define __ModulePhysics_H__

#include "Module.h"
#include "Globals.h"
#include"p2List_Extended.h"
#include "Box2D/Box2D/Box2D.h"
#include"ModuleAudio.h"
#include"Animation.h"

class b2World;
class b2Body;
struct b2Vec2;
typedef float float32;
typedef int int32;

//PhysBody class
class PhysBody
{
public:
	PhysBody() : listener(NULL), body(NULL)
	{}
	~PhysBody();


public:
	//Position and rotation functions
	int GetPositionPixels_X();
	int GetPositionPixels_Y();
	float GetRotation();
	b2Vec2 GetPositionMeters();

public:
	b2Body* body;
	Module* listener;

	//Sound and score
	int scoreOnHit = 0;
	int soundOnHit = -1;
	Animation idle;
	Animation hit;
	Animation current_animation;

	//Render settings
	int spriteSheet = -1;
	b2Vec2 offset;
	bool needs_Center = true;
	int pivotX = 0, pivotY = 0;
	SDL_RendererFlip flip = SDL_FLIP_NONE;
};

class ModulePhysics : public Module, public b2ContactListener
{
public:
	ModulePhysics(Application* app, bool start_enabled = true);
	~ModulePhysics();

	bool Start();
	update_status PreUpdate();
	update_status PostUpdate();
	bool CleanUp();

public:
	//Body creation functions
	PhysBody* Create_Circle(int _x, int _y, float meter_radius, b2BodyType type, float density, SDL_Rect sec = {0, 0, 0, 0}, int hit_score = 0, int sound = -1, Animation* hit_animation = nullptr);
	PhysBody* Create_Rectangle(SDL_Rect size, int type, float density, SDL_Rect sec = {0, 0, 0, 0}, float angle = 0, int hit_score = 0, int sound = -1, Animation* hit_animation = nullptr);
	PhysBody* Create_Chain(float, float, int[], int, b2Vec2[], SDL_Rect sec = { 0, 0, 0, 0 }, SDL_RendererFlip flip = SDL_FLIP_NONE);
	PhysBody* Create_Poly(float, float, int[], int, b2Vec2[], SDL_Rect sec = { 0, 0, 0, 0 }, b2BodyType type = b2BodyType::b2_dynamicBody, SDL_RendererFlip flip = SDL_FLIP_NONE, b2Vec2 offset = { 0, 0 }, float density = 1.0f, int hit_score = 0, int sound = -1, Animation* hit_animation = nullptr);
	PhysBody* Create_Rectangle_Sensor(SDL_Rect rectangle, float rotation, SDL_Rect sec = { 0, 0, 0, 0 }, SDL_RendererFlip flip = SDL_FLIP_NONE, Animation* hit_animation = nullptr, int hit_score = 0);
	PhysBody* Create_Circle_Sensor(int _x, int _y, float meter_radius, b2BodyType type, float density, SDL_Rect sec = { 0, 0, 0, 0 }, int hit_score = 0, int sound = -1, Animation* hit_animation = nullptr);

	//Joint creation functions
	b2RevoluteJoint* Create_Revolute_Joint(b2Body*, float, int, int);
	void BeginContact(b2Contact* contact);

	//World settings
	float32 timeStep = 1.0f / 60.f;
	int32 velocityIterations = 12;
	int32 positionIterations = 10;

public:

	//World lists
	p2List_Extended<PhysBody*> world_body_list;
	p2List_Extended<b2Body*> joint_body_list;
	p2List_Extended<PhysBody*> converter_list;

	void DestroyBody(b2Body*);
	bool MoveObjectSmooth(b2Vec2* position, b2Vec2 target_point,float32 speed);
	b2World* world;
	int freezeType = 0;

private:

	b2Body* jointBody;
	b2MouseJoint* mouse_joint;
	b2Body* ground;
	float time = 0;
	float freezeTimeLimit = 150;
};

#endif
