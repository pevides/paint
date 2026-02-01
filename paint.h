#ifndef PAINT_H
#define PAINT_H

#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

//CONFIGS
#define COLOR_PICKER_SIZE 30
#define GOAL_FRAME_RATE 60 
#define MAX_UNDO 5
#define BUFFER_SIZE (MAX_UNDO + 1)

//STRUCTS & GLOBAL VARIABLES
static uint32_t hex_Colors[] = {
    0x000000, 0x808080, 0xFFFFFF, 0xFF0000, 0xA52A2A, 0xFFA500, 0xFFFF00, 0x008000,
    0x00FFFF, 0x0000FF, 0x800080, 0xFF00FF, 0x00FF00, 0xFFC0CB, 0x008080, 0x000080
};
static int len = (int) (sizeof(hex_Colors) / sizeof(hex_Colors[0]));

typedef struct history {
    SDL_Surface* states[BUFFER_SIZE];
    int current;
    int total;
} history;

//FUNCTIONS
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
    SDL_SaveBMP(surface, "canvas.bmp");
}


void load_canvas(SDL_Surface* destination_surface) {
    SDL_Surface* source_surface = SDL_LoadBMP("canvas.bmp");
    SDL_BlitSurface(source_surface, NULL, destination_surface, NULL);
}

void history_init(history* h) {
    h->current = -1;
    h->total = 0;
    for(int i = 0; i < BUFFER_SIZE; i++) h->states[i] = NULL;
}


void push_to_history(history* h, SDL_Surface* surface) {

    //after making changes free states so its impossible to redo
    for (int i = h->current + 1; i < h->total; i++) {
        if (h->states[i % BUFFER_SIZE]) {
            SDL_FreeSurface(h->states[i % BUFFER_SIZE]);
            h->states[i % BUFFER_SIZE] = NULL;
        }
    }
    h->current++;

    int pos = h->current % BUFFER_SIZE;

    if (h->states[pos]) {
        SDL_FreeSurface(h->states[pos]);
    }

    h->total = h->current + 1;
    h->states[pos] = SDL_ConvertSurface(surface, surface->format, 0);
    printf("PUSH | Current: %d (Pos %d) | Total: %d\n", h->current, pos, h->total);
}

void undo(history* h, SDL_Surface* surface) {
    int oldest_possible = (h->total > BUFFER_SIZE) ? (h->total - BUFFER_SIZE) : 0;
    if (h->current > oldest_possible) {
        h->current--;
        SDL_BlitSurface(h->states[h->current % BUFFER_SIZE], NULL, surface, NULL);
        printf("UNDO -> Returned to Current: %d (Pos %d)\n", h->current, h->current % BUFFER_SIZE);
    } else {
        printf("UNDO -> Blocked (History limit achieved)\n");
    }
}

void redo(history* h, SDL_Surface* surface) {   
    if (h->current + 1 < h->total) {
        h->current++;
        int pos = h->current % BUFFER_SIZE;
        SDL_BlitSurface(h->states[pos], NULL, surface, NULL);
        printf("REDO -> advanced to Current: %d (Pos %d)\n", h->current, pos);
    } else {
        printf("REDO -> Nothing to redo\n");
    }
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

void change_brush_size(int* brush_size, int wheel_y, int wheel_x) {
    *brush_size += wheel_y * 10;
    *brush_size -= wheel_x * 10;
    if (*brush_size < 4) *brush_size = 4;
    if (*brush_size > 100) *brush_size = 100;

    printf("Brush size: %d\n", *brush_size);
}
#endif // PAINT_H