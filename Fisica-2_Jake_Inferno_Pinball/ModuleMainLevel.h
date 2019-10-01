#pragma once
#include "Module.h"
#include "Globals.h"
#include"Application.h"

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
		vector2 position;
		SDL_Rect section;
		float speed;
		float angle;

		int pivotX, pivotY;

	};

	//Speed 0 keeps object relative to the camera
	void LoadSpriteSheet(const char* load_path);
	void LoadSprite(int spriteSheetIndex,  float x, float y, SDL_Rect rect, float speed, float angle = 0.f, int pivotX = 0, int pivotY = 0);

	p2List_Extended<SDL_Texture*> sprite_sheet_list;
	p2List_Extended<Sprite> sprite_list;


	Sprite *rightPad, *leftPad;

};