#include "paint.h"

int main() {
    struct stat st = {0};
    if (stat("./temp", &st) == -1) {
        mkdir("./temp", 0755);
    }

    history hist;
    history_init(&hist);

    SDL_Window* window = SDL_CreateWindow("Paint", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, 0);
    SDL_Surface* surface = SDL_GetWindowSurface(window);    
    SDL_Rect rect = {0, 0, 1280, 720};
    SDL_FillRect(surface, &rect, 0xFFFFFF);
    SDL_UpdateWindowSurface(window);
    
    bool isRunning = true;
    bool draw = false;
    int mouse_x_pos, mouse_y_pos, last_x_pos, last_y_pos;
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
                        undo(&hist, surface);
                    }
                    if (ev.key.keysym.sym == SDLK_y && (lctrl || rctrl)) {
                        redo(&hist, surface);
                    }
                    if (ev.key.keysym.sym == SDLK_LCTRL) lctrl = true;
                    if (ev.key.keysym.sym == SDLK_RCTRL) rctrl = true;
                    if (ev.key.keysym.sym == SDLK_s) save_canvas(surface);
                    if (ev.key.keysym.sym == SDLK_l) load_canvas(surface);
                    break;
                case SDL_KEYUP:
                    if (ev.key.keysym.sym == SDLK_LCTRL) lctrl = false;
                    if (ev.key.keysym.sym == SDLK_RCTRL) rctrl = false;
                    break;
                
                case SDL_QUIT:
                    isRunning = false;
                    break;
                case SDL_MOUSEWHEEL:
                    change_brush_size(&brush_size, ev.wheel.y, ev.wheel.x);
                    break;
                case SDL_MOUSEMOTION:
                    mouse_x_pos = ev.motion.x;
                    mouse_y_pos = ev.motion.y;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (ev.button.button == SDL_BUTTON_LEFT) {
                        draw = true;
                        mouse_x_pos = ev.motion.x;
                        mouse_y_pos = ev.motion.y;
                        last_x_pos = mouse_x_pos;
                        last_y_pos = mouse_y_pos;
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    if (ev.button.button == SDL_BUTTON_LEFT) {
                        draw = false;
                        if (!is_inside_palette(mouse_x_pos, mouse_y_pos)) push_to_history(&hist, surface);
                    }
                    break;
            }
           
        }
        if (draw) {
            if (is_inside_palette(mouse_x_pos, mouse_y_pos)) {
                int col = mouse_x_pos / COLOR_PICKER_SIZE;
                int line = mouse_y_pos / COLOR_PICKER_SIZE;
                int i = line * 8 + col;
                color = hex_Colors[i];
                select_color(i, surface);
            } else {
                draw_line_of_circles(surface, last_x_pos, last_y_pos, mouse_x_pos, mouse_y_pos, brush_size, color);
                last_x_pos = mouse_x_pos;
                last_y_pos = mouse_y_pos;
            }
        }
        SDL_UpdateWindowSurface(window);
        SDL_Delay(1000/GOAL_FRAME_RATE);
    }

    for(int i = 0; i < BUFFER_SIZE; i++) {
        if(hist.states[i]) SDL_FreeSurface(hist.states[i]);
    }
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
 
}