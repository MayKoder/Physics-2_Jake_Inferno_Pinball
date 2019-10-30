#ifndef __ModuleFonts_H__
#define __ModuleFonts_H__

#include "Module.h"
#define MAX_FONTS 1
#define MAX_FONT_CHARS 256

struct Font_Data
{

	SDL_Texture* font_spritesheet;
	uint rows, len, char_w, char_h, row_chars;
	//62 supported chars
	//char* supportedChars = " !,.0123456789?ABCDEFGHIJKLMNOPQRSTUWYabcdefghiklmnopqrstuwxy'";
	char table[MAX_FONT_CHARS];

};

class ModuleFonts : public Module
{
public:

	ModuleFonts(Application* app, bool start_enabled = true);
	~ModuleFonts();

	bool Init();
	bool CleanUp();

	// Load Font
	int Load(const char* texture_path, const char* characters, uint rows, uint h, uint w, uint rc);
	void UnLoad(int font_id);

	// Create a surface from text
	void BlitText(int x, int y, int bmp_font_id, const char* text, int space = 0) const;


public:

	Font_Data main_font;
};



#endif
