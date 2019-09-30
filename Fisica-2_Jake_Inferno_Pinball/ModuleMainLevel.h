#pragma once
#include "Module.h"
#include "Globals.h"

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

		SDL_Texture* image;
		vector2 position;

	};

	p2List<Sprite> sprite_list;

};