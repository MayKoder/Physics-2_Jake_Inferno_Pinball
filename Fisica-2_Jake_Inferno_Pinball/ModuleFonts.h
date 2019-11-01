#ifndef __ModuleFonts_H__
#define __ModuleFonts_H__

#include "Module.h"
#define MAX_FONTS 1
#define MAX_FONT_CHARS 256

struct Font
{
	char table[MAX_FONT_CHARS];
	SDL_Texture* graphic = nullptr;
	uint len, char_w, char_h;
};

class ModuleFonts : public Module
{
public:

	ModuleFonts(Application* app, bool start_enabled = true);
	~ModuleFonts();

	bool Init();
	bool CleanUp();

	// Load Font
	int Load(const char* texture_path, const char* characters, int w = 0, int h = 0);
	void UnLoad(int font_id);

	// Create a surface from text
	void BlitText(int x, int y, int bmp_font_id, const char* text, SDL_Rect section, int lenght, int spacing) const;

public:

	Font	 fonts[MAX_FONTS];
};



#endif
