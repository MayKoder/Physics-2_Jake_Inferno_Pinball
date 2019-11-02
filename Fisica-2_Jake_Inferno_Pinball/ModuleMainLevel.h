#ifndef __ModuleMainLevel_H__
#define __ModuleMainLevel_H__

#include "Module.h"
#include "Globals.h"
#include"Application.h"
#include"ModulePhysics.h"
#include"p2SString.h"
#include<string>

struct Sprite;
class PhysBody;

class ModuleMainLevel : public Module
{
public:

	ModuleMainLevel(Application* app, bool start_enabled = true);
	~ModuleMainLevel();

	bool Start();
	update_status Update();
	bool CleanUp();

	//Coll detection
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);
	PhysBody* sensor;
	bool sensed;
	p2Point<int> ray;
	bool ray_on;

	const char* assets_path = "Assets/Main_Level/";

	struct Sprite 
	{
		int spriteSheetIndex;
		b2Vec2 position;
		SDL_Rect section;
		float speed;
		float angle;

		int pivotX, pivotY;

	};

	//Speed 0 keeps object relative to the camera
	void LoadSpriteSheet(const char* load_path);
	void LoadSprite(int spriteSheetIndex,  float x, float y, SDL_Rect rect, float speed, float angle = 0.f, int pivotX = 0, int pivotY = 0, int layer = 0);
	void SetBallOnSpawn(PhysBody*);

	void IncrementScore(int);
	void FormatScoreText(int score, char[]);

	PhysBody* Create_Play_Ball(int x, int y);


	p2List_Extended<SDL_Texture*> sprite_sheet_list;
	p2List_Extended<Sprite> gameplay_sprite_list;
	p2List_Extended<Sprite> cover_sprite_list;

//Gameplay refs
public:

	int ball_height_limit;
	int score = 0;
	int prevScore = 0;
	int highestScore = 0;

	char score_text[11] = {'0', '0', '0', ',', '0', '0', '0', ',', '0', '0', '0'};
	char prev_score_text[11] = {'0', '0', '0', ',', '0', '0', '0', ',', '0', '0', '0'};
	char highest_score_text[11] = {'0', '0', '0', ',', '0', '0', '0', ',', '0', '0', '0'};

	char lives_text[2] = { 'x', '0'};
	int current_ball_lives;

	void Lose_Ball(int);
	//void Shoot_Ball_From_Spawn();

	PhysBody* lower_Ball = nullptr;

	b2RevoluteJoint* righBumper[2];
	b2RevoluteJoint* leftBumper[2];
	Sprite* launchSpring;

	unsigned int bumper_up_sound;
	unsigned int bumper_down_sound;
	unsigned int spring_launch_sound;
	unsigned int spawn_lock_sound;
	unsigned int dragon_circle_sound;
	unsigned int green_circle_sound;
	unsigned int lateral_spring_sound;
	unsigned int red_stick_sound;
	unsigned int teleport_sound;


	bool springDown = false, springUp = false;



private: //Spawn settings//
	int max_ball_lives = 5;
	uint ballsOnScreen = 0;
	bool ball_in_spawn = false;
	PhysBody* ball_body_in_spawn;
	PhysBody* spawn_sensor = nullptr;
	PhysBody* spawn_blocker = nullptr;
	float32 launch_Force = 0;

};

#endif