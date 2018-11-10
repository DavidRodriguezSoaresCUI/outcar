#include "sdl2_func.h"

void log_SDL_error(const char *msg)
{
    SDL_Log("%s error: %s", msg, SDL_GetError());
}

SDL_Texture *load_texture(const char *file, SDL_Renderer *ren)
{
    // int to null to avoid dangling pointer
    SDL_Texture *texture = NULL;
    // Load the image
    SDL_Surface *loaded_image = IMG_Load(file);
    // if image loaded -> texture conversion and return
    if (loaded_image != NULL)
    {
        texture = SDL_CreateTextureFromSurface(ren, loaded_image);
        SDL_FreeSurface(loaded_image);
        // check conversion
        if (texture == NULL)
            log_SDL_error("CreateTextureFromSurface");
    } else
        log_SDL_error("LoadBMP");
    return texture;
}

/**
 * Draw a texture at x, y positions without scaling
 */
void render_texture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, SDL_Rect *clip)
{
    int w, h;
    // Get width and height of texture
    if (clip == NULL) SDL_QueryTexture(tex, NULL, NULL, &w, &h);
    else
    {
        w = clip->w;
        h = clip->h;
    }
    render_texture_scaling(tex, ren, x, y, w, h, clip);
}

/**
 * Draw a texture at x, y positions with scaling through w and h values
 */
void render_texture_scaling(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h,
                            SDL_Rect *clip)
{
    // create dest. drawing rectangle
    SDL_Rect dst;
    // set param. of rect.
    dst.x = x;
    dst.y = y;
    dst.w = w;
    dst.h = h;
    // render texture
    SDL_RenderCopy(ren, tex, clip, &dst);
}

/**
 * bitmap font rendering using only text and a single texture with only
 * capital letters of 8x8px ordered in a single horizontal line
 */
void
render_text_8x8_capital(SDL_Texture *font, SDL_Renderer *ren, char *text, int size, int x, int y)
{
    char *s = text;
    int i = 0;
    //int counter = 0;
    SDL_Rect clip;
    clip.y = 0;
    clip.w = 8;
    clip.h = 8;
    while (*s != '\0')
    {
        i = toupper((unsigned char) *s) - 32;
        clip.x = 8 * i;
        // NOTE if text is REALLY long, then (text-s) WILL overflow
        render_texture_scaling(font, ren, x + (size * (int) (s - text)), y, size, size, &clip);
        s++;
    }

}

/**
 * bitmap font rendering using only text and a single texture with only
 * capital letters of 8x8px ordered in a single horizontal line
 */

void render_text_8x8_capital_align_right(SDL_Texture *font, SDL_Renderer *ren, char *text,
                                         int size, int x, int y)
{
    char *s = text;
    int i = 0;
    size_t len = strlen(text);
    //int counter = 0;
    SDL_Rect clip;
    clip.y = 0;
    clip.w = 8;
    clip.h = 8;
    while (*s != '\0')
    {
        i = toupper((unsigned char) *s) - 32;
        clip.x = 8 * i;
        // NOTE if text is REALLY long, then (s-text) WILL overflow
        render_texture_scaling(font, ren, x - (size * (int) (len - (s-text))), y, size, size, &clip);
        s++;
    }

}

int render_lcd_clock(SDL_Texture *clock_texture, SDL_Renderer *ren, SDL_Rect clipsize, char *clock,
                     SDL_Rect render_area)
{
    if (strlen(clock) > 5)
        return -2;
    char *s = clock;
    clipsize.y = 0;
    int number_size = render_area.w / 5;
    int colon_size = render_area.w - (number_size * 4);
    int curr_x_pos = render_area.x;
    while (*s != '\0')
    {
        if (48 <= *s && *s <= 58)
        {
            clipsize.x = clipsize.w * (*s - 48);
            if (*s != 58)
            {
                render_texture_scaling(clock_texture, ren, curr_x_pos, render_area.y, number_size,
                                       render_area.h, &clipsize);
                curr_x_pos += number_size;
            } else
            {
                render_texture_scaling(clock_texture, ren, curr_x_pos, render_area.y, colon_size,
                                       render_area.h, &clipsize);
                curr_x_pos += colon_size;
            }

        } else
        {
            return -1;
        }
        s++;
    }
    return 0;
}
