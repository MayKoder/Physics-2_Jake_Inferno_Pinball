#include "Globals.h"
#include "Application.h"
#include"ModuleMainLevel.h"
#include"Box2D/Box2D/Box2D.h"

ModuleMainLevel::ModuleMainLevel(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleMainLevel::~ModuleMainLevel()
{}

void ModuleMainLevel::LoadSpriteSheet(const char* load_path)
{
	sprite_sheet_list.add(App->textures->Load(load_path));
}

void ModuleMainLevel::LoadSprite(int spriteSheetIndex, float posX, float posY, SDL_Rect rect, float speed, float angle, int pivotX, int pivotY)
{

	Sprite tempSprite;

	tempSprite.spriteSheetIndex = spriteSheetIndex;
	tempSprite.position.x = (float)posX;
	tempSprite.position.y = -(float)posY;

	tempSprite.section = rect;
	tempSprite.angle = angle;
	tempSprite.speed = speed;

	tempSprite.pivotX = pivotX;
	tempSprite.pivotY = pivotY;


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


	//TODO: Render everything with coliders?
#pragma region Chain Initials

	// Pivot 0, 0
	int points[216] = {
			0, 0,
			328, 0,
			328, 1004,
			323, 1004,
			323, 650,
			321, 639,
			319, 628,
			316, 619,
			313, 609,
			309, 600,
			304, 590,
			298, 581,
			292, 573,
			286, 566,
			280, 559,
			273, 552,
			264, 544,
			256, 539,
			247, 533,
			240, 529,
			231, 525,
			221, 521,
			216, 520,
			212, 518,
			210, 514,
			210, 505,
			214, 501,
			293, 469,
			297, 464,
			299, 460,
			299, 362,
			275, 338,
			275, 277,
			305, 233,
			305, 142,
			302, 124,
			297, 108,
			293, 98,
			288, 88,
			283, 79,
			277, 70,
			269, 61,
			262, 54,
			255, 47,
			249, 42,
			243, 38,
			237, 34,
			231, 30,
			224, 26,
			215, 22,
			206, 19,
			198, 17,
			191, 15,
			184, 14,
			176, 13,
			167, 12,
			147, 12,
			136, 14,
			124, 17,
			111, 21,
			100, 26,
			89, 32,
			79, 39,
			70, 45,
			62, 52,
			55, 60,
			48, 68,
			42, 77,
			37, 85,
			31, 96,
			27, 106,
			24, 116,
			21, 125,
			20, 134,
			19, 147,
			19, 235,
			36, 277,
			36, 339,
			13, 362,
			13, 460,
			14, 463,
			17, 467,
			22, 471,
			31, 474,
			98, 501,
			101, 505,
			101, 514,
			98, 517,
			94, 520,
			88, 523,
			81, 527,
			72, 534,
			64, 541,
			56, 549,
			49, 556,
			43, 564,
			38, 573,
			33, 582,
			28, 593,
			24, 606,
			21, 617,
			19, 631,
			19, 741,
			45, 774,
			45, 835,
			12, 864,
			12, 1003,
			0, 1003
	};
	b2Vec2 half_Array[(sizeof(points) / sizeof(int)) / 2];
	App->physics->world_body_list.add(App->physics->CreateChain(8, -(1000 - (SCREEN_HEIGHT - 12)), *&points, (sizeof(points) / sizeof(int)), *&half_Array));

	// Pivot 0, 0
	int cover_left[14] = {
		31, 1004,
		105, 1004,
		105, 1002,
		41, 972,
		37, 972,
		34, 973,
		31, 976
	};
	half_Array[(sizeof(cover_left) / sizeof(int)) / 2];
	App->physics->world_body_list.add(App->physics->CreateChain(8, -(1000 - (SCREEN_HEIGHT - 12)), *&cover_left, (sizeof(cover_left) / sizeof(int)), *&half_Array));	
	
	// Pivot 0, 0
	int cover_right[12] = {
		205, 1004,
		279, 1004,
		279, 976,
		277, 973,
		272, 971,
		238, 987
	};
	half_Array[(sizeof(cover_right) / sizeof(int)) / 2];
	App->physics->world_body_list.add(App->physics->CreateChain(8, -(1000 - (SCREEN_HEIGHT - 12)), *&cover_right, (sizeof(cover_right) / sizeof(int)), *&half_Array));
	

	// Pivot 0, 0
	int launchCover[16] = {
		300, 1003,
		300, 860,
		277, 837,
		275, 834,
		275, 774,
		303, 744,
		307, 744,
		307, 1003
	};
	half_Array[(sizeof(launchCover) / sizeof(int)) / 2];
	App->physics->world_body_list.add(App->physics->CreateChain(8, -(1000 - (SCREEN_HEIGHT - 12)), *&launchCover, (sizeof(launchCover) / sizeof(int)), *&half_Array));


	// Pivot 0, 0
	int slim_stick_left[20] = {
		31, 885,
		31, 940,
		35, 944,
		93, 971,
		96, 969,
		95, 966,
		41, 941,
		36, 938,
		36, 885,
		34, 884
	};
	half_Array[(sizeof(slim_stick_left) / sizeof(int)) / 2];
	App->physics->world_body_list.add(App->physics->CreateChain(8, -(1000 - (SCREEN_HEIGHT - 12)), *&slim_stick_left, (sizeof(slim_stick_left) / sizeof(int)), *&half_Array));
	App->physics->world_body_list.add(App->physics->CreateChain(8, -(1510 - SCREEN_HEIGHT), *&slim_stick_left, (sizeof(slim_stick_left) / sizeof(int)), *&half_Array));


	// Pivot 0, 0
	int slim_stick_right[18] = {
		221, 971,
		279, 944,
		283, 940,
		283, 886,
		280, 883,
		277, 884,
		277, 939,
		219, 966,
		218, 969
	};
	half_Array[(sizeof(slim_stick_right) / sizeof(int)) / 2];
	App->physics->world_body_list.add(App->physics->CreateChain(8, -(1000 - (SCREEN_HEIGHT - 12)), *&slim_stick_right, (sizeof(slim_stick_right) / sizeof(int)), *&half_Array));
	App->physics->world_body_list.add(App->physics->CreateChain(8, -(1510 - SCREEN_HEIGHT), *&slim_stick_right, (sizeof(slim_stick_right) / sizeof(int)), *&half_Array));

#pragma endregion




	//HitSticks
	LoadSprite(0, 102, (-SCREEN_HEIGHT) + 43, { 27, 287, 50, 28 }, 1.f, 0, 0);
	LoadSprite(0, 179, (-SCREEN_HEIGHT) + 43, { 77, 287, 50, 28 }, 1.f, 0, 100);
	leftPad = &sprite_list[sprite_list.count() - 2];
	rightPad = &sprite_list[sprite_list.count() - 1];


	//Screen Cover
	LoadSprite(0, 0, 0, { 0, 0, 580, 287 }, 0.f);



	App->renderer->posY_Limit = (sprite_list[0].section.h * SCREEN_SIZE) - ((SCREEN_HEIGHT - 16 ) * SCREEN_SIZE);



	return true;
}

// Update: draw background
struct b2Vec2;
update_status ModuleMainLevel::Update()
{

	if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN) 
	{
		App->physics->world_body_list.add(App->physics->Create_Circle(App->input->GetMouseX(),App->input->GetMouseY(), PIXELS_TO_METERS(13/2), 2, 1.f, 0, { 0, 360, 13, 13 }));
	}
	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN)
	{
		App->physics->world_body_list.add(App->physics->Create_Circle(App->input->GetMouseX(), App->input->GetMouseY(), PIXELS_TO_METERS(13/2), 1, 0.f, 0, { 0, 360, 13, 13 }));
	}
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT) {
		leftPad->angle -= 5;
	}
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT) {
		rightPad->angle += 5;
	}

	for (int i = 0; i < sprite_list.count(); i++)
	{
		Sprite forSprite = sprite_list[i];
		App->renderer->Blit(sprite_sheet_list[forSprite.spriteSheetIndex], forSprite.position.x, forSprite.position.y, &forSprite.section, forSprite.speed, forSprite.angle, forSprite.pivotX, forSprite.pivotY);
	}


	//PHS Draw
	for (int i = 0; i < App->physics->world_body_list.count(); i++)
	{

		if (App->physics->world_body_list[i].GetPositionPixels_Y() >= 500)
		{

			App->physics->DestroyBody(App->physics->world_body_list[i].body);
			App->physics->world_body_list.del(App->physics->world_body_list.At(i));
			LOG("Body deleted")
			//Destroy balls on fall or just reposition them?
		}
		else
		{
			if (App->physics->world_body_list[i].spriteSheet != -1)
			{
				BodyClass *temp = &App->physics->world_body_list[i];

				if (temp->needs_Center)
				{
					//LOG("%i", temp.GetRotation())
					App->renderer->Blit(sprite_sheet_list[temp->spriteSheet], (temp->GetPositionPixels_X()) - (temp->section.w / 2), (temp->GetPositionPixels_Y()) - (temp->section.h / 2), &temp->section, 1.f, temp->GetRotation());
				}
				else
				{
					App->renderer->Blit(sprite_sheet_list[temp->spriteSheet], temp->GetPositionPixels_X(), temp->GetPositionPixels_Y(), &temp->section, 1.f, temp->GetRotation());
				}
			}
		}
		LOG("%i", App->physics->world_body_list.count());

		//0, 360, 13, 13
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




