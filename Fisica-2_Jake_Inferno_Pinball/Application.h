#pragma once

#include "p2List.h"
#include"p2List_Extended.h"
#include "Globals.h"
#include "Module.h"
#include "Dummy.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModulePlayer.h"
#include "ModulePhysics.h"
#include"ModuleMainLevel.h"
#include"ModuleFonts.h"

class Application
{
public:
	ModuleRender* renderer;
	ModuleWindow* window;
	ModuleTextures* textures;
	ModuleInput* input;
	ModuleAudio* audio;
	ModulePlayer* player;
	ModuleMainLevel* main_level;
	ModulePhysics* physics;
	ModuleFonts* fonts;

private:

	p2List<Module*> list_modules;

public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();
	char IntToChar(int num) {
		return (num + '0');
	}

private:

	void AddModule(Module* mod);
};