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

	Load("Assets/Fonts/font_normal.png", " !,.0123456789?ABCDEFGHIJKLMNOPQRSTUWYabcdefghiklmnopqrstuwxy'", 8);


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
int ModuleFonts::Load(const char* texture_path, const char* characters, uint rows)
{
	int id = 0;

	if (texture_path == nullptr || characters == nullptr || rows == 0)
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
	fonts[id].rows = rows; // rows: rows of characters in the texture
	fonts[id].len = strlen(characters); // len: length of the table

	// table: array of chars to have the list of characters
	for (uint i = 0; i < MAX_FONT_CHARS; i++) {
		fonts[id].table[i] = characters[i];
	}


	uint width, height;
	App->textures->GetSize(tex, width, height);
	// row_chars: amount of chars per row of the texture
	fonts[id].row_chars = strlen(characters) / fonts[id].rows;
	// char_w: width of each character
	fonts[id].char_w = width / fonts[id].row_chars;  //10
	// char_h: height of each character
	fonts[id].char_h = height / rows;

	LOG("Successfully loaded BMP font from %s", texture_path);

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
void ModuleFonts::BlitText(int x, int y, int font_id, const char* text) const
{
	if (text == nullptr || font_id < 0 || font_id >= MAX_FONTS || fonts[font_id].graphic == nullptr)
	{
		LOG("Unable to render text with bmp font id %d", font_id);
		return;
	}

	const Font* font = &fonts[font_id];
	SDL_Rect rect;
	uint len = strlen(text);
	
	rect.w = font->char_w;
	rect.h = font->char_h;
	rect.y = 0;

	SDL_Rect rect_copy = rect;
	for (uint i = 0; i < len; ++i)
	{
		//Looking through table to compare character against char from our text introduced.
		for (uint j = 0; j < fonts[font_id].row_chars; ++j) 
		{
			if (fonts[font_id].table[j] == text[i]) 
			{
				rect.x = j * (fonts[font_id].char_w / 1.3);

				if (j == 2) 
				{
					rect.w -= 17;
					x += 7;
				}

				App->renderer->DrawQuad({x, y, rect.w, rect.h}, 255, 255, 255, 50);
				App->renderer->Blit(fonts[font_id].graphic, x - (38 * i), y, &rect, false);
				x += rect.w;
				rect = rect_copy;
			}
		}

	}

	/*for (int i = 0; i < len; ++i)
	{
		for (int j = 0; j < MAX_FONT_CHARS; j++) {
			if (text[i] == font->table[j]) {
				rect.x = j * font->char_w;
				rect.y = 0;
				if (font_id == 0 && j == 36) { y -= 2; }
				App->renderer->Blit(main_font.graphic, x, y, &rect, 0);
				y = y2;
				x += font->char_w;
				break;
			}
			if (j > font->row_chars) { break; }
		}
	}*/
}