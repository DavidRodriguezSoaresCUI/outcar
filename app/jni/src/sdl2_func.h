#ifndef SDL2FUNC
    #define SDL2FUNC

    #include <stdio.h>
    #include <stdlib.h>
    #include <SDL.h>
    #include <SDL_image.h>

    #define cleanup(x) _Generic((x),             \
                SDL_Window*: SDL_DestroyWindow,   \
              SDL_Renderer*: SDL_DestroyRenderer, \
               SDL_Texture*: SDL_DestroyTexture,  \
               SDL_Surface*: SDL_FreeSurface)(x)

    void log_SDL_error(const char *msg);

    SDL_Texture *load_texture(const char *file, SDL_Renderer *ren);

    void render_texture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, SDL_Rect *clip);

    void render_texture_scaling(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h,
                                SDL_Rect *clip);

    void render_text_8x8_capital(SDL_Texture *font, SDL_Renderer *ren, char *text, int size,
                                 int x, int y);

    void render_text_8x8_capital_align_right(SDL_Texture *font, SDL_Renderer *ren, char *text,
                                         int size, int x, int y);

    int render_lcd_clock(SDL_Texture *clock_texture, SDL_Renderer *ren, SDL_Rect clipsize,
                         char *clock, SDL_Rect render_area);

#endif
