#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>

#define COLOR_PICKER_SIZE 30
#define GOAL_FRAME_RATE 60 
uint32_t hex_Colors[] = {
    0x000000, 0x808080, 0xFFFFFF, 0xFF0000, 0xA52A2A, 0xFFA500, 0xFFFF00, 0x008000,
    0x00FFFF, 0x0000FF, 0x800080, 0xFF00FF, 0x00FF00, 0xFFC0CB, 0x008080, 0x000080
};
int len = (int) (sizeof(hex_Colors) / sizeof(hex_Colors[0]));

typedef struct last_paint {
    int x, y;
    int brush_size;
} last_paint;

int square(int x) {
    return x * x;
}

void paint_color_picker(SDL_Surface* surface) {
    int color_x = 0; 
    int color_y = 0;

    for (int i = 0; i < len; i++) {
    SDL_FillRect(surface, &(const SDL_Rect){ color_x, color_y, COLOR_PICKER_SIZE, COLOR_PICKER_SIZE}, hex_Colors[i]);
    color_x += COLOR_PICKER_SIZE;
    if ((i + 1) % 8 == 0) {
        color_y += COLOR_PICKER_SIZE;
        color_x = 0;
    }
}
}

void save_canvas(SDL_Surface* surface) {
    SDL_SaveBMP(surface, "temp.bmp");
}


void load_canvas(SDL_Surface* destination_surface) {
    SDL_Surface* source_surface = SDL_LoadBMP("temp.bmp");
    SDL_BlitSurface(source_surface, NULL, destination_surface, NULL);
}

void select_color(int i, SDL_Surface* surface) {
    paint_color_picker(surface);
    int x = (i % 8) * COLOR_PICKER_SIZE;
    int y = (i / 8) * COLOR_PICKER_SIZE;
    SDL_FillRect(surface, &(const SDL_Rect){ x, y, COLOR_PICKER_SIZE, COLOR_PICKER_SIZE / 6}, 0x0);
    SDL_FillRect(surface, &(const SDL_Rect){ x, COLOR_PICKER_SIZE * (i / 8 + 1) - COLOR_PICKER_SIZE / 6, COLOR_PICKER_SIZE, COLOR_PICKER_SIZE / 6}, 0x0);
    SDL_FillRect(surface, &(const SDL_Rect){ x, y, COLOR_PICKER_SIZE / 6, COLOR_PICKER_SIZE}, 0x0);
    SDL_FillRect(surface, &(const SDL_Rect){ COLOR_PICKER_SIZE * (i % 8 + 1) - COLOR_PICKER_SIZE / 6, y, COLOR_PICKER_SIZE / 6, COLOR_PICKER_SIZE}, 0x0);
}
void draw_circle(int x, int y, int radius, int color, SDL_Surface* surface) {
    
    //(x - a)^2 + (y + b)^2 <= radius^2
    for (int a = x - radius; a <= x + radius; a++) {
        for (int b = y - radius; b <= y + radius; b++) {
            if ( square(x - a) + square( y - b) <= square(radius) && !(a < 8 * COLOR_PICKER_SIZE && b <( len / 8) * COLOR_PICKER_SIZE)) {
                SDL_FillRect(surface, &(const SDL_Rect){ a, b, 1, 1}, color);
            }
        }
    }
    return;
}

int main() {
    printf("Hello Paint\n");
    SDL_Window* window = SDL_CreateWindow("Paint", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, 0);
    SDL_Surface* surface = SDL_GetWindowSurface(window);
    SDL_Rect rect = {0, 0, 1280, 720};
    SDL_FillRect(surface, &rect, 0xFFFFFF);
    SDL_UpdateWindowSurface(window);
    bool isRunning = true;
    bool draw = false;
    int x, y;
    SDL_Event ev;
    int brush_size = 4;
    bool lctrl = false;
    bool rctrl = false;
    
    int32_t color;
    paint_color_picker(surface);

    while (isRunning) {
        while(SDL_PollEvent(&ev) != 0) {
            switch (ev.type) {
                case SDL_KEYDOWN:
                    if (ev.key.keysym.sym == SDLK_z && (lctrl || rctrl)) {
                        load_canvas(surface);
                    }
                    if (ev.key.keysym.sym == SDLK_LCTRL) lctrl = true;
                    if (ev.key.keysym.sym == SDLK_RCTRL) rctrl = true;
                    break;
                case SDL_KEYUP:
                    if (ev.key.keysym.sym == SDLK_LCTRL) lctrl = false;
                    if (ev.key.keysym.sym == SDLK_RCTRL) rctrl = false;
                    break;
                
                case SDL_QUIT:
                    isRunning = false;
                    break;
                case SDL_MOUSEWHEEL:
                    brush_size += ev.wheel.y * 10;
                    brush_size -= ev.wheel.x * 10;
                    if (brush_size < 4) brush_size = 4;
                    if (brush_size > 100) brush_size = 100;
                    break;
                case SDL_MOUSEMOTION:
                    x = ev.motion.x;
                    y = ev.motion.y;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (ev.button.button == SDL_BUTTON_LEFT) {
                        save_canvas(surface);
                        draw = true;
                        x = ev.motion.x;
                        y = ev.motion.y;
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    if (ev.button.button == SDL_BUTTON_LEFT) draw = false;
                    break;
            }
           
        }
        if (draw) {
            if (x < 8 * COLOR_PICKER_SIZE && y < (len / 8) * COLOR_PICKER_SIZE) {
                int col = x / COLOR_PICKER_SIZE;
                int line = y / COLOR_PICKER_SIZE;
                int i = line * 8 + col;
                color = hex_Colors[i];
                select_color(i, surface);
            }
            else draw_circle(x, y, brush_size, color, surface);
        }
        SDL_UpdateWindowSurface(window);
        SDL_Delay(1000/GOAL_FRAME_RATE);
    }
    SDL_DestroyWindow(window);
    SDL_Quit();
 
}