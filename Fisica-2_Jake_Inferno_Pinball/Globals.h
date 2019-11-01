#pragma once
#include <windows.h>
#include <stdio.h>

#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);

void log(const char file[], int line, const char* format, ...);

#define CAP(n) ((n <= 0.0f) ? n=0.0f : (n >= 1.0f) ? n=1.0f : n=n)

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f


#define PIXELS_PER_METER 50.0f // if touched change METER_PER_PIXEL too
#define METER_PER_PIXEL 0.02f // this is 1 / PIXELS_PER_METER !

#define METERS_TO_PIXELS(m) ((int) floor(PIXELS_PER_METER * m))
#define PIXELS_TO_METERS(p)  ((float) METER_PER_PIXEL * p)

typedef unsigned int uint;

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

struct vector2 {
	float x;
	float y;
};
struct vector3 
{
	int x;
	int y;
	int z;

	void Init_Zeros() {
		x = 0; y = 0; z = 0;
	}

	void Increment(int increment) 
	{
		if (z + increment >= 999) 
		{


		}
		else
		{
			z += increment;
		}
	}

};

// Configuration -----------
#define SCREEN_WIDTH 580
#define SCREEN_HEIGHT 287
//To use fullscreen, set size to 1
#define SCREEN_SIZE 2
#define WIN_FULLSCREEN false
#define WIN_RESIZABLE false
#define WIN_BORDERLESS false
#define WIN_FULLSCREEN_DESKTOP false
#define VSYNC true
#define TITLE "2D Physics Playground"
#define MARGIN_X 18
#define MARGIN_Y 8