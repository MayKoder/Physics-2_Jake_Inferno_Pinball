#include "Globals.h"
#include "Application.h"
#include"ModuleMainLevel.h"
#include"Box2D/Box2D/Box2D.h"
#include"p2SString.h"
#include"Animation.h"

ModuleMainLevel::ModuleMainLevel(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	ray_on = false;
}

ModuleMainLevel::~ModuleMainLevel()
{}

void ModuleMainLevel::LoadSpriteSheet(const char* load_path)
{
	sprite_sheet_list.add(App->textures->Load(load_path));
}

void ModuleMainLevel::LoadSprite(int spriteSheetIndex, float posX, float posY, SDL_Rect rect, float speed, float angle, int pivotX, int pivotY, int layer)
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


	if (layer == 0) 
	{
		gameplay_sprite_list.add(tempSprite);
	}
	else
	{
		cover_sprite_list.add(tempSprite);
	}

}

// Load assets
bool ModuleMainLevel::Start()
{
	LOG("Loading player");


	//Audio loading
	bumper_down_sound = App->audio->LoadFx("Assets/Audio/fx/bumper_down.wav");
	bumper_up_sound = App->audio->LoadFx("Assets/Audio/fx/bumper_up.wav");
	spring_launch_sound = App->audio->LoadFx("Assets/Audio/fx/spring_launch.wav");
	spawn_lock_sound = App->audio->LoadFx("Assets/Audio/fx/spawn_closing.wav");
	round_start_sound = App->audio->LoadFx("Assets/Audio/fx/start_sound.wav");
	dragon_circle_sound = App->audio->LoadFx("Assets/Audio/fx/dragon_circle_sound.wav");
	green_circle_sound = App->audio->LoadFx("Assets/Audio/fx/green_circle_sound.wav");
	lateral_spring_sound = App->audio->LoadFx("Assets/Audio/fx/lateral_spring_sound.wav");
	red_stick_sound = App->audio->LoadFx("Assets/Audio/fx/red_stick_sound.wav");
	teleport_sound = App->audio->LoadFx("Assets/Audio/fx/teleport_sound.wav");
	big_triangle_sound = App->audio->LoadFx("Assets/Audio/fx/big_triangle_hit.wav");

	//height balls will be deleted
	ball_height_limit = (SCREEN_HEIGHT * SCREEN_SIZE); //+20
	current_ball_lives = max_ball_lives;
	lives_text[1] = App->IntToChar(current_ball_lives);

	LoadSpriteSheet("Assets/Main_Level/main_level_static_background.png");
	
	//Load all ANIMATIONS
	GlobalAnimationLoad();

	//Loads all SPRITES
	GlobalSpriteLoad();

	//Loads all map OBJECTS
	GlobalMapLoad();


	SetBallOnSpawn(Create_Play_Ball(324, 60));
	App->renderer->posY_Limit = (gameplay_sprite_list[0].section.h * SCREEN_SIZE) - ((SCREEN_HEIGHT - 16 ) * SCREEN_SIZE);

	return true;
}

// Update: draw background
struct b2Vec2;
update_status ModuleMainLevel::Update()
{

	//TODO: Delete this, just for map building
	if(App->input->debug)
		LOG("X = %i, Y = %i", App->input->GetMouseX(), App->input->GetMouseY());

	if(lower_Ball)
		App->renderer->MoveCameraToPosition(lower_Ball->GetPositionPixels_Y());

	if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN) 
	{
		Create_Play_Ball(App->input->GetMouseX(), App->input->GetMouseY());
	}
	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN)
	{
		App->physics->world_body_list.add(App->physics->Create_Circle(App->input->GetMouseX(), App->input->GetMouseY(), PIXELS_TO_METERS(13/2), b2BodyType::b2_dynamicBody, 0.f, { 0, 360, 13, 13 }));
	}

	//Left Bumper Movement
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT && leftBumper)
	{
		//leftBumper->GetBodyB()->ApplyForceToCenter({ 0, -50 }, true);
		leftBumper[0]->GetBodyB()->ApplyForce({ 0, -30 }, {PIXELS_TO_METERS(150), PIXELS_TO_METERS(269)}, true);
		leftBumper[1]->GetBodyB()->ApplyForce({ 0, -30 }, {PIXELS_TO_METERS(150), PIXELS_TO_METERS(269)}, true);
	}

	//Bumper sound
	if(App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)
	{
		App->audio->PlayFx(bumper_up_sound);
	}
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_UP || App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_UP)
	{
		App->audio->PlayFx(bumper_down_sound);
	}

	//Right Bumper Movement
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT && righBumper)
	{
		righBumper[0]->GetBodyB()->ApplyForce({ 0, -30 }, { PIXELS_TO_METERS(181), PIXELS_TO_METERS(269) }, true);
		righBumper[1]->GetBodyB()->ApplyForce({ 0, -30 }, { PIXELS_TO_METERS(181), PIXELS_TO_METERS(269) }, true);
	}

	//LOG("%i", launchSpring->position.x)
	//Down key movement
	if (ball_in_spawn) 
	{
		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN && ball_body_in_spawn) 
		{
			springDown = true;
			springUp = false;
			launch_Force = 0;
		}
		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_UP && ball_body_in_spawn)
		{
			App->audio->PlayFx(spring_launch_sound);
			springDown = false;
			springUp = true;
		}
	}

	if (springDown) 
	{
		springDown = !App->physics->MoveObjectSmooth(&launchSpring->position, { 0, 255 }, 1);
	}
	if (springUp) 
	{
		springUp = !App->physics->MoveObjectSmooth(&launchSpring->position, { 0, 209 }, 9);
		launch_Force += 10.f;
		if (!springUp) 
		{
			//LOG("%f", launch_Force);
			ball_body_in_spawn->body->ApplyForceToCenter({0, -launch_Force}, true);
		}
	}


	//Gameplay sprite renderer
	for (int i = 0; i < (int)gameplay_sprite_list.count(); i++)
	{
		Sprite *forSprite = &gameplay_sprite_list[i];
		App->renderer->Blit(sprite_sheet_list[forSprite->spriteSheetIndex], (int)forSprite->position.x, (int)forSprite->position.y, &forSprite->section, forSprite->speed, forSprite->angle, forSprite->pivotX, forSprite->pivotY);
	}


	//PHS Object renderer
	for (int i = 0; i < (int)App->physics->world_body_list.count(); i++)
	{

		//Follow lower ball
		if(App->physics->world_body_list[i]->body->GetType() == b2BodyType::b2_dynamicBody && App->physics->world_body_list[i]->body->GetFixtureList()->GetType() == b2Shape::e_circle)
		{
			//LOG("%i", App->physics->world_body_list[i].GetPositionPixels_Y());
			if(lower_Ball == nullptr)
			{
				lower_Ball = App->physics->world_body_list[i];
			}
			else if(App->physics->world_body_list[i]->GetPositionPixels_Y() > lower_Ball->GetPositionPixels_Y())
			{
				lower_Ball = App->physics->world_body_list[i];
			}
		}

		//Is object out of map limits?
		if (App->physics->world_body_list[i]->GetPositionPixels_Y() >= ball_height_limit)
		{
			if (App->physics->world_body_list[i]->body->GetFixtureList()->GetType() == b2Shape::e_circle)
				Lose_Ball(i);
			//Destroy balls on fall or just reposition them?
		}
		else
		{
			if (App->physics->world_body_list[i]->spriteSheet != -1)
			{
				PhysBody *temp = App->physics->world_body_list[i];

				if (temp->body->IsActive())
				{
					SDL_Rect frame = temp->current_animation.GetCurrentFrame();


					if (temp->current_animation.render_on_top)
					{

						if (temp->needs_Center)
						{
							//LOG("%i", temp.GetRotation())
							App->renderer->Blit(sprite_sheet_list[temp->spriteSheet],
								(temp->GetPositionPixels_X()) - (frame.w / 2),
								(temp->GetPositionPixels_Y()) - (frame.h / 2),
								&temp->idle.GetCurrentFrameWithoutAnim(),
								1.f,
								temp->GetRotation(),
								INT_MAX, INT_MAX,
								temp->flip);

							App->renderer->Blit(sprite_sheet_list[temp->spriteSheet],
								(temp->GetPositionPixels_X()) - (frame.w / 2),
								(temp->GetPositionPixels_Y()) - (frame.h / 2),
								&frame,
								1.f,
								temp->GetRotation(),
								INT_MAX, INT_MAX,
								temp->flip);
						}
						else
						{
							App->renderer->Blit(sprite_sheet_list[temp->spriteSheet],
								temp->GetPositionPixels_X() - (int)temp->offset.x,
								temp->GetPositionPixels_Y() - (int)temp->offset.x,
								&temp->idle.GetCurrentFrameWithoutAnim(),
								1.f,
								temp->GetRotation(),
								temp->pivotX,
								temp->pivotY,
								temp->flip);

							App->renderer->Blit(sprite_sheet_list[temp->spriteSheet],
								temp->GetPositionPixels_X() - (int)temp->offset.x,
								temp->GetPositionPixels_Y() - (int)temp->offset.x,
								&frame,
								1.f,
								temp->GetRotation(),
								temp->pivotX,
								temp->pivotY,
								temp->flip);
						}

					}
					else
					{

						if (temp->needs_Center)
						{
							App->renderer->Blit(sprite_sheet_list[temp->spriteSheet],
								(temp->GetPositionPixels_X()) - (frame.w / 2),
								(temp->GetPositionPixels_Y()) - (frame.h / 2),
								&frame,
								1.f,
								temp->GetRotation(),
								INT_MAX, INT_MAX,
								temp->flip);
						}
						else
						{
							App->renderer->Blit(sprite_sheet_list[temp->spriteSheet],
								temp->GetPositionPixels_X() - (int)temp->offset.x,
								temp->GetPositionPixels_Y() - (int)temp->offset.x,
								&frame,
								1.f,
								temp->GetRotation(),
								temp->pivotX,
								temp->pivotY,
								temp->flip);
						}
					}

					if (temp->current_animation.finished)
					{
						temp->current_animation = temp->idle;
					}
				}
			}
		}
		//LOG("%i", App->physics->world_body_list.count());
	}

	//COver post gameplay screen renderer
	for (int i = 0; i < (int)cover_sprite_list.count(); i++)
	{
		Sprite *forSprite = &cover_sprite_list[i];
		App->renderer->Blit(sprite_sheet_list[forSprite->spriteSheetIndex], (int)forSprite->position.x, (int)forSprite->position.y, &forSprite->section, forSprite->speed, forSprite->angle, forSprite->pivotX, forSprite->pivotY);
	}

	//Render UI elements
	int lives_Sprite_Position = 480;
	for (int i = 0; i < current_ball_lives; i++)
	{
		SDL_Rect sec = { 0, 360, 13, 13 };
		App->renderer->Blit(sprite_sheet_list[0], lives_Sprite_Position, 129, &sec, 0.f);
		lives_Sprite_Position -= sec.w + 9;
	}


	//Print UI
	App->fonts->BlitText(410, 170, 2, "Last Score", { 0, 0, 8, 14 }, 14, 11);
	App->fonts->BlitText(398, 225, 2, "Highest Score", { 0, 0, 8, 14 }, 13, 11);
	App->fonts->BlitText(349, 59, 0, score_text, {0, 0, 20, 24}, 11, 2);
	App->fonts->BlitText(498, 124, 0, lives_text, { 0, 0, 12, 23 }, 2, 8);

	App->fonts->BlitText(387, 190, 0, prev_score_text, { 0, 0, 12, 20 }, 11, 7);
	App->fonts->BlitText(387, 246, 0, highest_score_text, { 0, 0, 12, 20 }, 11, 7);

	if (level_ended == 0)
	{
		if (fadeValue < 255)
		{
			App->renderer->DrawQuad({ 0, 0, SCREEN_WIDTH* SCREEN_SIZE, SCREEN_HEIGHT* SCREEN_SIZE }, 0, 0, 0, fadeValue, true, false);
			fadeValue += 5;
		}
		else
		{
			level_ended = 2;

			current_ball_lives = 0;
			prevScore = score;
			if (score > highestScore)
			{
				highestScore = score;
			}
			score = 0;
			FormatScoreText(prevScore, prev_score_text);
			FormatScoreText(highestScore, highest_score_text);
			FormatScoreText(score, score_text);
			current_ball_lives = max_ball_lives;
			SetBallOnSpawn(App->physics->world_body_list[ballPos]);

			App->renderer->DrawQuad({ 0, 0, SCREEN_WIDTH* SCREEN_SIZE, SCREEN_HEIGHT* SCREEN_SIZE }, 0, 0, 0, fadeValue, true, false);
			//Aqui resetear tot
		}
	}
	else if (level_ended == 2)
	{
		if (fadeValue > 0)
		{
			App->renderer->DrawQuad({ 0, 0, SCREEN_WIDTH * SCREEN_SIZE, SCREEN_HEIGHT* SCREEN_SIZE }, 0, 0, 0, fadeValue, true, false);
			fadeValue -= 2;
		}
		else
		{
			level_ended = -1;
			fadeValue = 0;
			//Aqui resetear tot
		}
	}

	return UPDATE_CONTINUE;
}

// Unload assets
bool ModuleMainLevel::CleanUp()
{
	LOG("Unloading player");

	for (int i = 0; i < (int)sprite_sheet_list.count(); i++)
	{
		App->textures->Unload(sprite_sheet_list[i]);
		//delete sprite_list[i].section;
	}
	sprite_sheet_list.clear();

	gameplay_sprite_list.clear();
	cover_sprite_list.clear();

	if (righBumper != nullptr) 
	{
		App->physics->world->DestroyJoint(righBumper[0]);
		righBumper[0] = nullptr;
		App->physics->world->DestroyJoint(righBumper[1]);
		righBumper[1] = nullptr;
	}
	if (leftBumper != nullptr) {
		App->physics->world->DestroyJoint(leftBumper[0]);
		leftBumper[0] = nullptr;
		App->physics->world->DestroyJoint(leftBumper[1]);
		leftBumper[1] = nullptr;
	}

	return true;
}

void ModuleMainLevel::Lose_Ball(int positionOnList)
{

	//is ball the only ball on the screen?
	if (ballsOnScreen == 1) 
	{
		if (current_ball_lives - 1 <= 0)
		{
			//End game
			level_ended = 0;
			ballPos = positionOnList;
		}
		else
		{
			current_ball_lives--;
			//Reset ball to spawn point
			SetBallOnSpawn(App->physics->world_body_list[positionOnList]);
		}
		lives_text[1] = App->IntToChar(current_ball_lives);

	}
	else
	{
		if (lower_Ball->body == App->physics->world_body_list[positionOnList]->body)
		{
			lower_Ball = nullptr;
		}

		App->physics->DestroyBody(App->physics->world_body_list[positionOnList]->body);
		App->physics->world_body_list.del(App->physics->world_body_list.At(positionOnList));
		ballsOnScreen--;	
		//LOG("%i", ballsOnScreen);
	}


}

PhysBody* ModuleMainLevel::Create_Play_Ball(int x, int y) 
{
	ballsOnScreen++;
	PhysBody* ret = App->physics->world_body_list.add(App->physics->Create_Circle(x, y, PIXELS_TO_METERS(13 / 2), b2BodyType::b2_dynamicBody, 1.f, { 0, 360, 13, 13 }))->data;
	return ret;
}

void ModuleMainLevel::SetBallOnSpawn(PhysBody* spawn_ball)
{
	App->audio->PlayFx(round_start_sound);
	spawn_ball->body->SetLinearVelocity({ 0, 0 });
	spawn_ball->body->SetAngularVelocity(0);
	spawn_ball->body->SetTransform({ PIXELS_TO_METERS(324), PIXELS_TO_METERS(150) }, 0);
	ball_body_in_spawn = spawn_ball;
	App->physics->converter_list.add(spawn_sensor);
	App->physics->converter_list.add(spawn_blocker);
	ball_in_spawn = true;
}


void ModuleMainLevel::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{

	if (bodyA && bodyB)
	{
		if (bodyB->scoreOnHit != 0)
		{
			bool prog = true;
			for (int i = 0; i < 8; i++)
			{
				if (bodyB == big_triangle_array[i])
				{
					prog = false;
				}
			}
			if (prog) 
			{
				IncrementScore(bodyB->scoreOnHit);
				if (bodyB->soundOnHit != -1)
				{
					App->audio->PlayFx(bodyB->soundOnHit);
				}

				if (bodyB->hit.last_frame > 0)
				{
					//bodyB->current_animation = bodyB->hit;
					bodyB->current_animation = bodyB->hit;
					//LOG("Anim");
				}
			}
			//LOG("%s", score_text);
		}

		for (int i = 0; i < 8; i++)
		{
			if (bodyB == big_triangle_array[i])
			{
				if (bodyB == big_triangle_array[1])
				{
					bodyB->current_animation = bodyB->hit;
					big_triangle_array[i - 1]->current_animation = big_triangle_array[i - 1]->hit;
					App->audio->PlayFx(big_triangle_array[i - 1]->soundOnHit);
					bodyA->body->ApplyLinearImpulse({ 0.4, -0.3 }, bodyA->body->GetWorldCenter(), true);

				}
			}
		}

		if (bodyB == teleport_enter)
		{
			App->audio->PlayFx(teleport_sound);
			bodyA->body->SetLinearVelocity({ 0, 0 });
			bodyA->body->SetAngularVelocity(0);
			App->physics->converter_list.add(bodyA);
			App->physics->converter_list.add(teleport_exits[0]);
		}

		if (bodyB == spawn_sensor)
		{
			LOG("Out of spawn");
			App->audio->PlayFx(spawn_lock_sound);
			App->physics->converter_list.add(spawn_sensor);
			App->physics->converter_list.add(spawn_blocker);
			launch_Force = 0;
			ball_in_spawn = false;
			ball_body_in_spawn = nullptr;
		}
	}
}

void ModuleMainLevel::FormatScoreText(int score, char score_txt[])
{
	p2SString* currenttext = new p2SString("%d", score);
	//currenttext->create("%d", score);
	const char* cur = currenttext->GetString();
	int zeros = 9 - currenttext->Length();
	int count = 0;
	//Text update
	for (int i = 0; i < 11; i++)
	{

		if (i == 3 || i == 7)
		{
			score_txt[i] = ',';
		}
		else
		{
			if (zeros > 0)
			{
				score_txt[i] = '0';
				zeros--;
			}
			else
			{
				score_txt[i] = cur[count];
				count++;
			}
		}

	}
	delete currenttext;
}

void ModuleMainLevel::IncrementScore(int increment) 
{
	if (score + increment <= 999999999)
	{
		score += increment;

		FormatScoreText(score, score_text);
	}
	else
	{
		score = 999999999;
		FormatScoreText(score, score_text);
	}
}


void ModuleMainLevel::GlobalMapLoad() 
{

#pragma region Map Construction

	//Dragon circles
	App->physics->world_body_list.add(App->physics->Create_Circle(103 + MARGIN_X, 0 - (1009 - SCREEN_HEIGHT) + 720 + MARGIN_Y, 0.28f, b2BodyType::b2_staticBody, 0.f, { 79, 287, 29, 33 }, 2000, dragon_circle_sound, &dragon_circle_anim));
	App->physics->world_body_list.add(App->physics->Create_Circle(195 + MARGIN_X, 0 - (1009 - SCREEN_HEIGHT) + 720 + MARGIN_Y, 0.28f, b2BodyType::b2_staticBody, 0.f, { 79, 287, 29, 33 }, 2000, dragon_circle_sound,&dragon_circle_anim));
	App->physics->world_body_list.add(App->physics->Create_Circle(52 + MARGIN_X, 0 - (1009 - SCREEN_HEIGHT) + 657 + MARGIN_Y, 0.28f, b2BodyType::b2_staticBody, 0.f, { 79, 287, 29, 33 }, 2000, dragon_circle_sound, &dragon_circle_anim));
	App->physics->world_body_list.add(App->physics->Create_Circle(234 + MARGIN_X, 0 - (1009 - SCREEN_HEIGHT) + 657 + MARGIN_Y, 0.28f, b2BodyType::b2_staticBody, 0.f, { 79, 287, 29, 33 }, 2000, dragon_circle_sound, &dragon_circle_anim));
	App->physics->world_body_list.add(App->physics->Create_Circle(195 + MARGIN_X, 0 - (1009 - SCREEN_HEIGHT) + 603 + MARGIN_Y, 0.28f, b2BodyType::b2_staticBody, 0.f, { 79, 287, 29, 33 }, 2000, dragon_circle_sound, &dragon_circle_anim));
	App->physics->world_body_list.add(App->physics->Create_Circle(103 + MARGIN_X, 0 - (1009 - SCREEN_HEIGHT) + 603 + MARGIN_Y, 0.28f, b2BodyType::b2_staticBody, 0.f, { 79, 287, 29, 33 }, 2000, dragon_circle_sound, &dragon_circle_anim));
	App->physics->world_body_list.add(App->physics->Create_Circle(212 + MARGIN_X, 0 - (1009 - SCREEN_HEIGHT) + 333 + MARGIN_Y, 0.28f, b2BodyType::b2_staticBody, 0.f, { 79, 287, 29, 33 }, 2000, dragon_circle_sound, &dragon_circle_anim));
	App->physics->world_body_list.add(App->physics->Create_Circle(142 + MARGIN_X, 0 - (1009 - SCREEN_HEIGHT) + 262 + MARGIN_Y, 0.28f, b2BodyType::b2_staticBody, 0.f, { 79, 287, 29, 33 }, 2000, dragon_circle_sound, &dragon_circle_anim));
	App->physics->world_body_list.add(App->physics->Create_Circle(69 + MARGIN_X, 0 - (1009 - SCREEN_HEIGHT) + 333 + MARGIN_Y, 0.28f, b2BodyType::b2_staticBody, 0.f, { 79, 287, 29, 33 }, 2000, dragon_circle_sound, &dragon_circle_anim));

	//Green circles
	App->physics->world_body_list.add(App->physics->Create_Circle_Sensor(250 + MARGIN_X, 0 - (1009 - SCREEN_HEIGHT) + 777 + MARGIN_Y, 0.16f, b2BodyType::b2_staticBody, 0.f, { 138, 326, 16, 16 }, 1000, green_circle_sound, &green_circle_anim));
	App->physics->world_body_list.add(App->physics->Create_Circle_Sensor(250 + MARGIN_X, 0 - (1009 - SCREEN_HEIGHT) + 796 + MARGIN_Y, 0.16f, b2BodyType::b2_staticBody, 0.f, { 138, 326, 16, 16 }, 1000, green_circle_sound, &green_circle_anim));
	App->physics->world_body_list.add(App->physics->Create_Circle_Sensor(250 + MARGIN_X, 0 - (1009 - SCREEN_HEIGHT) + 815 + MARGIN_Y, 0.16f, b2BodyType::b2_staticBody, 0.f, { 138, 326, 16, 16 }, 1000, green_circle_sound, &green_circle_anim));
	App->physics->world_body_list.add(App->physics->Create_Circle_Sensor(52 + MARGIN_X, 0 - (1009 - SCREEN_HEIGHT) + 777 + MARGIN_Y, 0.16f, b2BodyType::b2_staticBody, 0.f, { 138, 326, 16, 16 }, 1000, green_circle_sound, &green_circle_anim));
	App->physics->world_body_list.add(App->physics->Create_Circle_Sensor(52 + MARGIN_X, 0 - (1009 - SCREEN_HEIGHT) + 796 + MARGIN_Y, 0.16f, b2BodyType::b2_staticBody, 0.f, { 138, 326, 16, 16 }, 1000, green_circle_sound, &green_circle_anim));
	App->physics->world_body_list.add(App->physics->Create_Circle_Sensor(52 + MARGIN_X, 0 - (1009 - SCREEN_HEIGHT) + 815 + MARGIN_Y, 0.16f, b2BodyType::b2_staticBody, 0.f, { 138, 326, 16, 16 }, 1000, green_circle_sound, &green_circle_anim));
	App->physics->world_body_list.add(App->physics->Create_Circle_Sensor(149 + MARGIN_X, 0 - (1009 - SCREEN_HEIGHT) + 637 + MARGIN_Y, 0.16f, b2BodyType::b2_staticBody, 0.f, { 138, 326, 16, 16 }, 1000, green_circle_sound, &green_circle_anim));
	App->physics->world_body_list.add(App->physics->Create_Circle_Sensor(149 + MARGIN_X, 0 - (1009 - SCREEN_HEIGHT) + 694 + MARGIN_Y, 0.16f, b2BodyType::b2_staticBody, 0.f, { 138, 326, 16, 16 }, 1000, green_circle_sound, &green_circle_anim));
	App->physics->world_body_list.add(App->physics->Create_Circle_Sensor(72 + MARGIN_X, 0 - (1009 - SCREEN_HEIGHT) + 541 + MARGIN_Y, 0.16f, b2BodyType::b2_staticBody, 0.f, { 138, 326, 16, 16 }, 1000, green_circle_sound, &green_circle_anim));
	App->physics->world_body_list.add(App->physics->Create_Circle_Sensor(226 + MARGIN_X, 0 - (1009 - SCREEN_HEIGHT) + 541 + MARGIN_Y, 0.16f, b2BodyType::b2_staticBody, 0.f, { 138, 326, 16, 16 }, 1000, green_circle_sound, &green_circle_anim));
	App->physics->world_body_list.add(App->physics->Create_Circle_Sensor(198 + MARGIN_X, 0 - (1009 - SCREEN_HEIGHT) + 524 + MARGIN_Y, 0.16f, b2BodyType::b2_staticBody, 0.f, { 138, 326, 16, 16 }, 1000, green_circle_sound, &green_circle_anim));
	App->physics->world_body_list.add(App->physics->Create_Circle_Sensor(99 + MARGIN_X, 0 - (1009 - SCREEN_HEIGHT) + 524 + MARGIN_Y, 0.16f, b2BodyType::b2_staticBody, 0.f, { 138, 326, 16, 16 }, 1000, green_circle_sound, &green_circle_anim));
	App->physics->world_body_list.add(App->physics->Create_Circle_Sensor(249 + MARGIN_X, 0 - (1009 - SCREEN_HEIGHT) + 279 + MARGIN_Y, 0.16f, b2BodyType::b2_staticBody, 0.f, { 138, 326, 16, 16 }, 1000, green_circle_sound, &green_circle_anim));
	App->physics->world_body_list.add(App->physics->Create_Circle_Sensor(249 + MARGIN_X, 0 - (1009 - SCREEN_HEIGHT) + 298 + MARGIN_Y, 0.16f, b2BodyType::b2_staticBody, 0.f, { 138, 326, 16, 16 }, 1000, green_circle_sound, &green_circle_anim));
	App->physics->world_body_list.add(App->physics->Create_Circle_Sensor(249 + MARGIN_X, 0 - (1009 - SCREEN_HEIGHT) + 317 + MARGIN_Y, 0.16f, b2BodyType::b2_staticBody, 0.f, { 138, 326, 16, 16 }, 1000, green_circle_sound, &green_circle_anim));
	App->physics->world_body_list.add(App->physics->Create_Circle_Sensor(43 + MARGIN_X, 0 - (1009 - SCREEN_HEIGHT) + 279 + MARGIN_Y, 0.16f, b2BodyType::b2_staticBody, 0.f, { 138, 326, 16, 16 }, 1000, green_circle_sound, &green_circle_anim));
	App->physics->world_body_list.add(App->physics->Create_Circle_Sensor(43 + MARGIN_X, 0 - (1009 - SCREEN_HEIGHT) + 298 + MARGIN_Y, 0.16f, b2BodyType::b2_staticBody, 0.f, { 138, 326, 16, 16 }, 1000, green_circle_sound, &green_circle_anim));
	App->physics->world_body_list.add(App->physics->Create_Circle_Sensor(43 + MARGIN_X, 0 - (1009 - SCREEN_HEIGHT) + 317 + MARGIN_Y, 0.16f, b2BodyType::b2_staticBody, 0.f, { 138, 326, 16, 16 }, 1000, green_circle_sound, &green_circle_anim));

	//Red circles
	App->physics->world_body_list.add(App->physics->Create_Circle_Sensor(205 + MARGIN_X, -(1009 - SCREEN_HEIGHT) + 233 + MARGIN_Y, 0.16f, b2BodyType::b2_staticBody, 0.f, { 154, 326, 16, 16 }, 2000, green_circle_sound, &red_circle_anim));
	App->physics->world_body_list.add(App->physics->Create_Circle_Sensor(85 + MARGIN_X, -(1009 - SCREEN_HEIGHT) + 233 + MARGIN_Y, 0.16f, b2BodyType::b2_staticBody, 0.f, { 154, 326, 16, 16 }, 2000, green_circle_sound, &red_circle_anim));
	App->physics->world_body_list.add(App->physics->Create_Circle_Sensor(102 + MARGIN_X, -(1009 - SCREEN_HEIGHT) + 192 + MARGIN_Y, 0.16f, b2BodyType::b2_staticBody, 0.f, { 154, 326, 16, 16 }, 2000, green_circle_sound, &red_circle_anim));
	App->physics->world_body_list.add(App->physics->Create_Circle_Sensor(188 + MARGIN_X, -(1009 - SCREEN_HEIGHT) + 192 + MARGIN_Y, 0.16f, b2BodyType::b2_staticBody, 0.f, { 154, 326, 16, 16 }, 2000, green_circle_sound, &red_circle_anim));
	App->physics->world_body_list.add(App->physics->Create_Circle_Sensor(146 + MARGIN_X, -(1009 - SCREEN_HEIGHT) + 174 + MARGIN_Y, 0.16f, b2BodyType::b2_staticBody, 0.f, { 154, 326, 16, 16 }, 2000, green_circle_sound, &red_circle_anim));
	App->physics->world_body_list.add(App->physics->Create_Circle_Sensor(23 + MARGIN_X, -(1009 - SCREEN_HEIGHT) + 119 + MARGIN_Y, 0.16f, b2BodyType::b2_staticBody, 0.f, { 154, 326, 16, 16 }, 2000, green_circle_sound, &red_circle_anim));
	App->physics->world_body_list.add(App->physics->Create_Circle_Sensor(32 + MARGIN_X, -(1009 - SCREEN_HEIGHT) + 90 + MARGIN_Y, 0.16f, b2BodyType::b2_staticBody, 0.f, { 154, 326, 16, 16 }, 2000, green_circle_sound, &red_circle_anim));
	App->physics->world_body_list.add(App->physics->Create_Circle_Sensor(46 + MARGIN_X, -(1009 - SCREEN_HEIGHT) + 65 + MARGIN_Y, 0.16f, b2BodyType::b2_staticBody, 0.f, { 154, 326, 16, 16 }, 2000, green_circle_sound, &red_circle_anim));
	App->physics->world_body_list.add(App->physics->Create_Circle_Sensor(67 + MARGIN_X, -(1009 - SCREEN_HEIGHT) + 43 + MARGIN_Y, 0.16f, b2BodyType::b2_staticBody, 0.f, { 154, 326, 16, 16 }, 2000, green_circle_sound, &red_circle_anim));
	App->physics->world_body_list.add(App->physics->Create_Circle_Sensor(238 + MARGIN_X, -(1009 - SCREEN_HEIGHT) + 43 + MARGIN_Y, 0.16f, b2BodyType::b2_staticBody, 0.f, { 154, 326, 16, 16 }, 2000, green_circle_sound, &red_circle_anim));
	App->physics->world_body_list.add(App->physics->Create_Circle_Sensor(258 + MARGIN_X, -(1009 - SCREEN_HEIGHT) + 65 + MARGIN_Y, 0.16f, b2BodyType::b2_staticBody, 0.f, { 154, 326, 16, 16 }, 2000, green_circle_sound, &red_circle_anim));
	App->physics->world_body_list.add(App->physics->Create_Circle_Sensor(271 + MARGIN_X, -(1009 - SCREEN_HEIGHT) + 90 + MARGIN_Y, 0.16f, b2BodyType::b2_staticBody, 0.f, { 154, 326, 16, 16 }, 2000, green_circle_sound, &red_circle_anim));
	App->physics->world_body_list.add(App->physics->Create_Circle_Sensor(282 + MARGIN_X, -(1009 - SCREEN_HEIGHT) + 119 + MARGIN_Y, 0.16f, b2BodyType::b2_staticBody, 0.f, { 154, 326, 16, 16 }, 2000, green_circle_sound, &red_circle_anim));

	//Teleport 
	teleport_enter = App->physics->world_body_list.add(App->physics->Create_Circle_Sensor(146 + MARGIN_X, 0 - (1009 - SCREEN_HEIGHT) + 561 + MARGIN_Y, 0.09f, b2BodyType::b2_staticBody, 0.f, { 199, 290, 21, 21 }, 100000))->data;
	teleport_exits[0] = App->physics->world_body_list.add(App->physics->Create_Circle_Sensor(35 + MARGIN_X, 0 - (1009 - SCREEN_HEIGHT) + 355 + MARGIN_Y, 0.09f, b2BodyType::b2_staticBody, 0.f, { 199, 290, 21, 21 }))->data;
	teleport_exits[1] = App->physics->world_body_list.add(App->physics->Create_Circle_Sensor(255 + MARGIN_X, 0 - (1009 - SCREEN_HEIGHT) + 355 + MARGIN_Y, 0.09f, b2BodyType::b2_staticBody, 0.f, { 199, 290, 21, 21 }))->data;
	App->physics->world_body_list.add(App->physics->Create_Circle_Sensor(152 + MARGIN_X, -(1009 - SCREEN_HEIGHT) + 102 + MARGIN_Y, 0.09f, b2BodyType::b2_staticBody, 0.f, { 199, 290, 21, 21 }, 100000));

	//Red sticks
	App->physics->world_body_list.add(App->physics->Create_Rectangle({ 89 + MARGIN_X, -(1009 - SCREEN_HEIGHT) + 554 + MARGIN_Y, 6, 14 }, b2BodyType::b2_staticBody, 0, { 194, 324, 12, 32 }, 0, 1000, red_stick_sound, &red_stick_anim));
	App->physics->world_body_list.add(App->physics->Create_Rectangle({ 213 + MARGIN_X, -(1009 - SCREEN_HEIGHT) + 554 + MARGIN_Y, 6, 14 }, b2BodyType::b2_staticBody, 0, { 194, 324, 12, 32 }, 0, 1000, red_stick_sound, &red_stick_anim));
	App->physics->world_body_list.add(App->physics->Create_Rectangle({ 241 + MARGIN_X, -(1009 - SCREEN_HEIGHT) + 577 + MARGIN_Y, 6, 14 }, b2BodyType::b2_staticBody, 0, { 194, 324, 12, 32 }, 0, 1000, red_stick_sound, &red_stick_anim));
	App->physics->world_body_list.add(App->physics->Create_Rectangle({ 60 + MARGIN_X, -(1009 - SCREEN_HEIGHT) + 577 + MARGIN_Y, 6, 14 }, b2BodyType::b2_staticBody, 0, { 194, 324, 12, 32 }, 0, 1000, red_stick_sound, &red_stick_anim));
	App->physics->world_body_list.add(App->physics->Create_Rectangle({ 120 + MARGIN_X, -(1009 - SCREEN_HEIGHT) + 81 + MARGIN_Y, 6, 14 }, b2BodyType::b2_staticBody, 0, { 194, 324, 12, 32 }, 0, 1000, red_stick_sound, &red_stick_anim));
	App->physics->world_body_list.add(App->physics->Create_Rectangle({ 188 + MARGIN_X, -(1009 - SCREEN_HEIGHT) + 81 + MARGIN_Y, 6, 14 }, b2BodyType::b2_staticBody, 0, { 194, 324, 12, 32 }, 0, 1000, red_stick_sound, &red_stick_anim));

	//Lateral springs
	App->physics->world_body_list.add(App->physics->Create_Rectangle_Sensor({ 277 + MARGIN_X, -(1009 - SCREEN_HEIGHT) + 743 + MARGIN_Y , 10, 3 }, -45, {372, 328, 29, 11}));
	App->physics->world_body_list.add(App->physics->Create_Rectangle_Sensor({ 25 + MARGIN_X, -(1009 - SCREEN_HEIGHT) + 743 + MARGIN_Y , 10, 3 }, 50, { 372, 328, 29, 11 }));

	//Lateral green sensors
	App->physics->world_body_list.add(App->physics->Create_Rectangle_Sensor({ 42 + MARGIN_X, -(1009 - SCREEN_HEIGHT) + 796 + MARGIN_Y , 10, 3 }, 0, { 359, 328, 13, 50 }));
	App->physics->world_body_list.add(App->physics->Create_Rectangle_Sensor({ 259 + MARGIN_X, -(1009 - SCREEN_HEIGHT) + 796 + MARGIN_Y , 10, 3 }, 0, { 359, 328, 13, 50 }, SDL_FLIP_HORIZONTAL));

#pragma endregion

	spawn_sensor = App->physics->world_body_list.add(App->physics->Create_Rectangle_Sensor({ 323, -5, 10, 3 }, -45))->data; //735s
	spawn_blocker = App->physics->world_body_list.add(App->physics->Create_Rectangle({ 323, 15, 10, 3 }, b2BodyType::b2_staticBody, 0, { 140, 343, 24, 18 }, -45, 0))->data; //735s
	App->physics->converter_list.add(spawn_sensor);

#pragma region Chain Initials

	// Map 0, 0
	int points[216] = {
			0, 0, 328, 0, 328, 1004, 323, 1004, 323, 650, 321, 639, 319, 628, 316, 619, 313, 609, 309, 600, 304, 590, 298, 581, 292, 573, 286, 566,
			280, 559, 273, 552, 264, 544, 256, 539, 247, 533, 240, 529, 231, 525, 221, 521, 216, 520, 212, 518, 210, 514, 210, 505, 214, 501, 293, 469,
			297, 464, 299, 460, 299, 362, 275, 338, 275, 277, 305, 233, 305, 142, 302, 124, 297, 108, 293, 98, 288, 88, 283, 79, 277, 70, 269, 61,
			262, 54, 255, 47, 249, 42, 243, 38, 237, 34, 231, 30, 224, 26, 215, 22, 206, 19, 198, 17, 191, 15, 184, 14, 176, 13, 167, 12,
			147, 12, 136, 14, 124, 17, 111, 21, 100, 26, 89, 32, 79, 39, 70, 45, 62, 52, 55, 60, 48, 68, 42, 77, 37, 85, 31, 96, 27, 106, 24, 116, 21, 125,
			20, 134, 19, 147, 19, 235, 36, 277, 36, 339, 13, 362, 13, 460, 14, 463, 17, 467, 22, 471, 31, 474, 98, 501, 101, 505, 101, 514, 98, 517, 94, 520,
			88, 523, 81, 527, 72, 534, 64, 541, 56, 549, 49, 556, 43, 564, 38, 573, 33, 582, 28, 593, 24, 606, 21, 617, 19, 631, 19, 741, 45, 774, 45, 835, 12, 864, 12, 1003, 0, 1003
	};
	b2Vec2 half_Array[(sizeof(points) / sizeof(int)) / 2];
	App->physics->world_body_list.add(App->physics->Create_Chain(8, -(1000 - (SCREEN_HEIGHT - 12)), *&points, (sizeof(points) / sizeof(int)), *&half_Array));

	//Top Loop TODO
	//// Pivot 0, 0
	//int top_loop[40] = {
	//	54, 185,47, 169,43, 146,42, 128,45, 104,56, 76,68, 58,90, 38,114, 24,137, 
	//	17,174, 16,207, 24,230, 37,254, 62,268, 85,276, 107,278, 125,278, 145,275, 161,267, 185
	//};
	//half_Array[(sizeof(top_loop) / sizeof(int)) / 2];
	//App->physics->world_body_list.add(App->physics->Create_Chain(8, -(1000 - (SCREEN_HEIGHT - 12)), *&top_loop, (sizeof(top_loop) / sizeof(int)), *&half_Array));



	// CoverLowLeft 0, 0
	int cover_left[14] = {
		31, 1004, 105, 1004, 105, 1002, 41, 972,
		37, 972, 34, 973, 31, 976
	};
	half_Array[(sizeof(cover_left) / sizeof(int)) / 2];
	App->physics->world_body_list.add(App->physics->Create_Chain(8, -(1000 - (SCREEN_HEIGHT - 12)), *&cover_left, (sizeof(cover_left) / sizeof(int)), *&half_Array));

	// CoverLowRight 0, 0
	int cover_right[12] = {
		205, 1004, 279, 1004, 279, 976,
		277, 973, 272, 971, 238, 987
	};
	half_Array[(sizeof(cover_right) / sizeof(int)) / 2];
	App->physics->world_body_list.add(App->physics->Create_Chain(8, -(1000 - (SCREEN_HEIGHT - 12)), *&cover_right, (sizeof(cover_right) / sizeof(int)), *&half_Array));


	// LauncherCover 0, 0
	int launchCover[16] = {
		300, 1003, 300, 860, 277, 837, 275, 834,
		275, 774, 303, 744, 307, 744, 307, 1003
	};
	half_Array[(sizeof(launchCover) / sizeof(int)) / 2];
	App->physics->world_body_list.add(App->physics->Create_Chain(8, -(1000 - (SCREEN_HEIGHT - 12)), *&launchCover, (sizeof(launchCover) / sizeof(int)), *&half_Array));


	// Slims Left 0, 0
	int slim_stick_left[20] = {
		31, 885, 31, 940, 35, 944, 93, 971, 96, 969,
		95, 966, 41, 941, 36, 938, 36, 885, 34, 884
	};
	half_Array[(sizeof(slim_stick_left) / sizeof(int)) / 2];
	App->physics->world_body_list.add(App->physics->Create_Chain(8, -(1000 - (SCREEN_HEIGHT - 12)), *&slim_stick_left, (sizeof(slim_stick_left) / sizeof(int)), *&half_Array));
	App->physics->world_body_list.add(App->physics->Create_Chain(8, -(1510 - SCREEN_HEIGHT), *&slim_stick_left, (sizeof(slim_stick_left) / sizeof(int)), *&half_Array));


	// Slims right 0, 0
	int slim_stick_right[18] = {
		221, 971, 279, 944, 283, 940, 283, 886,
		280, 883, 277, 884, 277, 939, 219, 966, 218, 969
	};
	half_Array[(sizeof(slim_stick_right) / sizeof(int)) / 2];
	App->physics->world_body_list.add(App->physics->Create_Chain(8, -(1000 - (SCREEN_HEIGHT - 12)), *&slim_stick_right, (sizeof(slim_stick_right) / sizeof(int)), *&half_Array));
	App->physics->world_body_list.add(App->physics->Create_Chain(8, -(1510 - SCREEN_HEIGHT), *&slim_stick_right, (sizeof(slim_stick_right) / sizeof(int)), *&half_Array));

	//Big triangles
	int big_triangle_1[10] = {
		12, 4, 46, 61, 40, 68,
		4, 51, 4, 7
	};
	big_triangle_array[0] = App->physics->world_body_list.add(App->physics->Create_Poly(58, 153, *&big_triangle_1,
		(sizeof(big_triangle_1) / sizeof(int)), *&half_Array, { 31, 324, 45, 72 }, b2BodyType::b2_staticBody, SDL_FLIP_NONE, { -5, 0 }, 1.f, 2700, big_triangle_sound, &big_triangle_anim))->data;
	big_triangle_array[1] = App->physics->world_body_list.add(App->physics->Create_Rectangle_Sensor({ 86, 186, 30, 3 }, 58))->data;

	//Triangle sensor here

	big_triangle_array[2] = App->physics->world_body_list.add(App->physics->Create_Poly(58, -345, *&big_triangle_1,
		(sizeof(big_triangle_1) / sizeof(int)), *&half_Array, { 31, 324, 45, 72 }, b2BodyType::b2_staticBody, SDL_FLIP_NONE, { -5, 0 }, 1.f, 2700, big_triangle_sound, &big_triangle_anim))->data;

	//Big triangles
	int big_triangle_2[10] = {
		40, 5,
		7, 62,
		14, 67,
		49, 51,
		49, 7
	};

	big_triangle_array[4] = App->physics->world_body_list.add(App->physics->Create_Poly(219, 153, *&big_triangle_2,
		(sizeof(big_triangle_2) / sizeof(int)), *&half_Array, { 31, 324, 45, 72 }, b2BodyType::b2_staticBody, SDL_FLIP_HORIZONTAL, { -5, 0 }, 1.f, 2700, big_triangle_sound, &big_triangle_anim))->data;


	big_triangle_array[6] = App->physics->world_body_list.add(App->physics->Create_Poly(219, -345, *&big_triangle_2,
		(sizeof(big_triangle_2) / sizeof(int)), *&half_Array, { 31, 324, 45, 72 }, b2BodyType::b2_staticBody, SDL_FLIP_HORIZONTAL, { -5, 0 }, 1.f, 2700, big_triangle_sound, &big_triangle_anim))->data;


	//BUMPERS	
	int bumper_left[14] = {
		4, 14, 0, 10, 1, 4,
		5, 0, 11, 0, 51, 25, 49, 29
	};
	half_Array[(sizeof(bumper_left) / sizeof(int)) / 2];
	App->physics->world_body_list.add(App->physics->Create_Poly(105, SCREEN_HEIGHT - 43, *&bumper_left,
		(sizeof(bumper_left) / sizeof(int)), *&half_Array, { 27, 287, 50, 28 }, b2BodyType::b2_dynamicBody));
	leftBumper[0] = App->physics->Create_Revolute_Joint(App->physics->world_body_list[App->physics->world_body_list.count() - 1]->body, -45, 102 + 10, SCREEN_HEIGHT - 43 + 7);

	App->physics->world_body_list.add(App->physics->Create_Poly(105, -254, *&bumper_left,
		(sizeof(bumper_left) / sizeof(int)), *&half_Array, { 27, 287, 50, 28 }, b2BodyType::b2_dynamicBody));
	leftBumper[1] = App->physics->Create_Revolute_Joint(App->physics->world_body_list[App->physics->world_body_list.count() - 1]->body, -45, 102 + 10, -254 + 7);


	int bumper_right[14] = {
		39, 0, 0, 24, 0, 27, 4, 27,
		48, 10, 49, 4, 46, 0
	};
	half_Array[(sizeof(bumper_right) / sizeof(int)) / 2];
	App->physics->world_body_list.add(App->physics->Create_Poly(178, SCREEN_HEIGHT - 43, *&bumper_right,
		(sizeof(bumper_right) / sizeof(int)), *&half_Array, { 27, 287, 50, 28 }, b2BodyType::b2_dynamicBody, SDL_FLIP_HORIZONTAL));
	righBumper[0] = App->physics->Create_Revolute_Joint(App->physics->world_body_list[App->physics->world_body_list.count() - 1]->body, 45, 220, SCREEN_HEIGHT - 43 + 7);

	App->physics->world_body_list.add(App->physics->Create_Poly(178, -254, *&bumper_right,
		(sizeof(bumper_right) / sizeof(int)), *&half_Array, { 27, 287, 50, 28 }, b2BodyType::b2_dynamicBody, SDL_FLIP_HORIZONTAL));
	righBumper[1] = App->physics->Create_Revolute_Joint(App->physics->world_body_list[App->physics->world_body_list.count() - 1]->body, 45, 220, -254 + 7);


	App->physics->world_body_list.add(App->physics->Create_Rectangle({ 323, SCREEN_HEIGHT - 42, 7, 35 }, b2BodyType::b2_staticBody, 0.f));

#pragma endregion
}

void ModuleMainLevel::GlobalSpriteLoad() 
{
	//Background
	LoadSprite(0, 11, 1000 - (SCREEN_HEIGHT - 8), { 580, 0, 322, 1000 }, 1.f);

	//Spring
	LoadSprite(0, 327 - 7, (-SCREEN_HEIGHT) + 73 + 5, { 17, 287, 10, 73 }, 1.f);
	//TODO: Millorar aquest sistema
	launchSpring = &gameplay_sprite_list[gameplay_sprite_list.count() - 1];

	//SpringCover
	LoadSprite(0, 327 - 10, (-SCREEN_HEIGHT) + 73 + 5, { 0, 287, 17, 73 }, 1.f);


	//Background Top
	LoadSprite(0, 11, 1000 - (SCREEN_HEIGHT - 9), { 902, 0, 322, 1000 }, 1.f, 0.f, 0, 0, 1);

	//Screen Cover
	LoadSprite(0, 0, 0, { 0, 0, 580, 287 }, 0.f, 0.f, 0, 0, 1);

	//
	LoadSprite(0, 394, -3, { 376, 568, 122, 116 }, 0.f, 0.f, 0, 0, 1);

	//Score Display
	LoadSprite(0, SCREEN_WIDTH - 221 - 10, 0, { 0, 567, 221, 153 }, 0.f, 0.f, 0, 0, 1);

	//More score covers
	LoadSprite(0, 387, -185, { 221, 567,152, 29 }, 0, 0, 0, 0, 1);
	LoadSprite(0, 387, -240, { 221, 567,152, 29 }, 0, 0, 0, 0, 1);
}

void ModuleMainLevel::GlobalAnimationLoad() 
{
	big_triangle_anim.PushBack({ 222, 324, 45, 72 });
	big_triangle_anim.render_on_top = true;
	big_triangle_anim.speed = 0.15f;

	red_circle_anim.PushBack({ 154, 361, 16, 16 });
	red_circle_anim.render_on_top = true;
	red_circle_anim.speed = 0.01f;

	green_circle_anim.PushBack({138, 361, 16, 16});
	green_circle_anim.render_on_top = true;
	green_circle_anim.speed = 0.01f;


	dragon_circle_anim.PushBack({108, 287, 29, 33});
	dragon_circle_anim.PushBack({138, 287, 29, 33});
	dragon_circle_anim.PushBack({167, 287, 29, 33});
	dragon_circle_anim.PushBack({ 79, 287, 29, 33 });
	dragon_circle_anim.render_on_top = true;
	dragon_circle_anim.speed = 0.21f;

	red_stick_anim.PushBack({207, 324, 12, 32});
	red_stick_anim.PushBack({ 194, 324, 12, 32 });
	red_stick_anim.PushBack({ 207, 324, 12, 32 });
	red_stick_anim.render_on_top = true;
	red_stick_anim.speed = 0.1f;
}