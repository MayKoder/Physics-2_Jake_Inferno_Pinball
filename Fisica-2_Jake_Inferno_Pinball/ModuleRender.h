#pragma once
#include "Module.h"
#include "Globals.h"

class ModuleRender : public Module
{
public:
	ModuleRender(Application* app, bool start_enabled = true);
	~ModuleRender();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

	bool Blit(SDL_Texture* texture, int x, int y, SDL_Rect* section = NULL, float speed = 1.0f, double angle = 0, int pivot_x = INT_MAX, int pivot_y = INT_MAX, SDL_RendererFlip flip = SDL_FLIP_NONE);
	bool DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool filled = true, bool use_camera = true);
	bool DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool use_camera = true);
	bool DrawCircle(int x1, int y1, int redius, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool use_camera = true);

	enum dir {
		up, down
	};

	void MoveCamera(int y, dir movDir);

	void MoveCameraToPosition(int target_y)
	{

		if (-(target_y -( SCREEN_HEIGHT / 2)) * SCREEN_SIZE  >= posY_Limit) 
		{
			camera.y = posY_Limit;
			return;
		}

		if (target_y >= negY_Limit + (SCREEN_HEIGHT / 2))
		{
			camera.y = negY_Limit;
			return;
		}
		
		camera.y = -(target_y - SCREEN_HEIGHT / 2) * SCREEN_SIZE;

	}


public:
	SDL_Renderer* renderer;
	SDL_Rect camera;
	int cameraSpeed = 3;


	int posY_Limit;
	int negY_Limit = 0;


};