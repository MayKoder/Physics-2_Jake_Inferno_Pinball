#pragma once
#include "Module.h"
#include "Globals.h"
#include"Application.h"
#include"ModulePhysics.h"

struct Sprite;

class ModuleMainLevel : public Module
{
public:

	ModuleMainLevel(Application* app, bool start_enabled = true);
	~ModuleMainLevel();

	bool Start();
	update_status Update();
	bool CleanUp();

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
	void Create_Play_Ball(int x, int y);


	p2List_Extended<SDL_Texture*> sprite_sheet_list;
	p2List_Extended<Sprite> gameplay_sprite_list;
	p2List_Extended<Sprite> cover_sprite_list;

//Gameplay refs
public:

	int ball_height_limit;
	int score = 000000000;
	int current_ball_lives;

	void Lose_Ball(int);
	//void Shoot_Ball_From_Spawn();

	BodyClass* lower_Ball = nullptr;

	b2RevoluteJoint* righBumper;
	b2RevoluteJoint* leftBumper;
	Sprite* launchSpring;

	bool springDown = false, springUp = false;



private:
	int max_ball_lives = 5;
	uint ballsOnScreen = 0;
	bool ball_in_spawn = false;

};