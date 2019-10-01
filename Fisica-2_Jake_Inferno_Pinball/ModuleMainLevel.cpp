#include "Globals.h"
#include "Application.h"
#include"ModuleMainLevel.h"

ModuleMainLevel::ModuleMainLevel(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleMainLevel::~ModuleMainLevel()
{}

void ModuleMainLevel::LoadSpriteSheet(const char* load_path)
{
	sprite_sheet_list.add(App->textures->Load(load_path));
}

void ModuleMainLevel::LoadSprite(int spriteSheetIndex, float posX, float posY, SDL_Rect rect, float speed)
{

	Sprite tempSprite;

	tempSprite.spriteSheetIndex = spriteSheetIndex;
	tempSprite.position.x = (float)posX;
	tempSprite.position.y = -(float)posY;

	tempSprite.section = rect;

	tempSprite.speed = speed;


	sprite_list.add(tempSprite);

}

// Load assets
bool ModuleMainLevel::Start()
{
	LOG("Loading player");

	LoadSpriteSheet("Assets/Main_Level/main_level_static_background.png");

	//Background
	LoadSprite(0, 11, 1000 - (SCREEN_HEIGHT - 8), { 580, 0, 322, 1000 }, 1.f);

	//Spring
	LoadSprite(0, 327 - 7, (-SCREEN_HEIGHT) + 73 + 5, { 17, 287, 10, 73 }, 1.f);

	//SpringCover
	LoadSprite(0, 327 - 10, (-SCREEN_HEIGHT) + 73 + 5, { 0, 287, 17, 73 }, 1.f);

	//Background Top
	LoadSprite(0, 11, 1000 - (SCREEN_HEIGHT - 9), { 902, 0, 322, 1000 }, 1.f);

	//Screen Cover
	LoadSprite(0, 0, 0, { 0, 0, 580, 287 }, 0.f);



	App->renderer->posY_Limit = (sprite_list[0].section.h * SCREEN_SIZE) - ((SCREEN_HEIGHT - 16 ) * SCREEN_SIZE);



	return true;
}

// Update: draw background
update_status ModuleMainLevel::Update()
{

	for (int i = 0; i < sprite_list.count(); i++)
	{
		Sprite forSprite = sprite_list[i];
		App->renderer->Blit(sprite_sheet_list[forSprite.spriteSheetIndex], forSprite.position.x, forSprite.position.y, &forSprite.section, forSprite.speed);
	}


	return UPDATE_CONTINUE;
}

// Unload assets
bool ModuleMainLevel::CleanUp()
{
	LOG("Unloading player");

	for (int i = 0; i < sprite_sheet_list.count(); i++)
	{
		App->textures->Unload(sprite_sheet_list[i]);
		//delete sprite_list[i].section;
		sprite_sheet_list.clear();
	}

	for (int i = 0; i < sprite_list.count(); i++)
	{
		sprite_sheet_list.clear();
	}



	return true;
}




