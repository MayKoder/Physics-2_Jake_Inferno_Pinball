#include "Globals.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleRender.h"
#include "ModuleFonts.h"

#include<string.h>

ModuleFonts::ModuleFonts(Application* app, bool start_enabled) : Module(app, start_enabled)
{}

// Destructor
ModuleFonts::~ModuleFonts()
{}

//Called before render is available
bool ModuleFonts::Init()
{
	LOG("Loading Fonts");
	bool ret = true;

	//App->textures->Load("Assets/Fonts/font_normal.png");

	Load("Assets/Fonts/font_normal.png", " !,.0123456789?ABCDEFGHIJKLMNOPQRSTUWYabcdefghiklmnopqrstuwxy'", 20, 31);
	Load("Assets/Fonts/font_normal_black.png", " !,.0123456789?ABCDEFGHIJKLMNOPQRSTUWYabcdefghiklmnopqrstuwxy'", 20, 31);
	Load("Assets/Fonts/font_normal_white.png", " !,.0123456789?ABCDEFGHIJKLMNOPQRSTUWYabcdefghiklmnopqrstuwxy'", 20, 31);


	return ret;
}

// Called before quitting
bool ModuleFonts::CleanUp()
{

	for (int i = 0; i < MAX_FONTS; i++)
	{
		UnLoad(i);
	}

	return true;
}


// Load new texture from file path
int ModuleFonts::Load(const char* texture_path, const char* characters, int w, int h)
{
	int id = loadedFonts;

	if (texture_path == nullptr || characters == nullptr)
	{
		LOG("Could not load font");
		return id;
	}

	SDL_Texture* tex = App->textures->Load(texture_path);

	if (tex == nullptr || strlen(characters) >= MAX_FONT_CHARS)
	{
		LOG("Could not load font at %s with characters '%s'", texture_path, characters);
		return id;
	}


	if (id == MAX_FONTS)
	{
		LOG("Cannot load font %s. Array is full (max %d).", texture_path, MAX_FONTS);
		return id;
	}

	fonts[id].graphic = tex; // graphic: pointer to the texture
	fonts[id].len = strlen(characters); // len: length of the table

	// table: array of chars to have the list of characters
	for (uint i = 0; i < MAX_FONT_CHARS; i++) {
		fonts[id].table[i] = characters[i];
	}
	// char_w: width of each character
	fonts[id].char_w = w;  //10
	// char_h: height of each character
	fonts[id].char_h = h;

	LOG("Successfully loaded BMP font from %s", texture_path);
	loadedFonts++;

	return id;
}

void ModuleFonts::UnLoad(int font_id)
{
	if (font_id >= 0 && font_id < MAX_FONTS && fonts[font_id].graphic != nullptr)
	{
		App->textures->Unload(fonts[font_id].graphic);
		fonts[font_id].graphic = nullptr;
		LOG("Successfully Unloaded BMP font_id %d", font_id);
	}
}

//// Render text using a bitmap font
void ModuleFonts::BlitText(int x, int y, int font_id, const char* text, SDL_Rect section, int lenght, int spacing) const
{
	if (text == nullptr || font_id < 0 || font_id >= MAX_FONTS || fonts[font_id].graphic == nullptr)
	{
		LOG("Unable to render text with bmp font id %d", font_id);
		return;
	}

	const Font* font = &fonts[font_id];
	SDL_Rect rect;
	uint input_len = lenght;
	
	rect.x = 0;
	rect.y = 0;
	rect.w = font->char_w;
	rect.h = font->char_h;

	SDL_Rect rect_copy = rect;
	for (uint i = 0; i < input_len; ++i)
	{
		//Looking through table to compare character against char from our text introduced.
		for (uint j = 0; j < fonts[font_id].len; ++j) 
		{
			
			if (fonts[font_id].table[j] == text[i]) 
			{
				rect.x = rect.w * j;
				x += rect.w - spacing;
				App->renderer->Blit(fonts[font_id].graphic, x, y, &rect, 0.f, 0, INT_MAX, INT_MAX, SDL_FLIP_NONE, &section);
				
				if (text[i] == ',') 
				{
					x -= 7;
				}
				if (text[i] == 'i') {
					x -= spacing / 2;
				}
			}
			//rect = rect_copy;
		}

	}
}