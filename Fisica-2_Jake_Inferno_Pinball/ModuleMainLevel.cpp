#include "Globals.h"
#include "Application.h"
#include"ModuleMainLevel.h"

ModuleMainLevel::ModuleMainLevel(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleMainLevel::~ModuleMainLevel()
{}

// Load assets
bool ModuleMainLevel::Start()
{
	LOG("Loading player");
	return true;
}

// Update: draw background
update_status ModuleMainLevel::Update()
{

	for (int i = 0; i < sprite_list.count(); i++)
	{
		App->renderer->Blit(sprite_list[i].image, 0, 0);
	}


	return UPDATE_CONTINUE;
}

// Unload assets
bool ModuleMainLevel::CleanUp()
{
	LOG("Unloading player");

	for (int i = 0; i < sprite_list.count(); i++)
	{
		App->textures->Unload(sprite_list[i].image);
		sprite_list.clear();
	}


	return true;
}




