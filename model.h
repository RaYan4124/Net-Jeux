// SDL2 Model by aurelien.esnard@u-bordeaux.fr

#ifndef MODEL_H
#define MODEL_H

#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct Env_t Env;

/* **************************************************************** */
     
#ifdef __ANDROID__ 
#define PRINT(STR, ...) do { SDL_Log(STR, ##__VA_ARGS__);  } while(0)
#define ERROR(STR, ...) do { SDL_Log(STR, ##__VA_ARGS__); exit(EXIT_FAILURE); } while(0)
#else
#define PRINT(STR, ...) do { printf(STR, ##__VA_ARGS__); } while(0)
#define ERROR(STR, ...) do { fprintf(stderr, STR, ##__VA_ARGS__); exit(EXIT_FAILURE); } while(0)
#endif

/* **************************************************************** */
     
#define APP_NAME "SDL2 Demo"
#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 600
#define DELAY 100
#define CELLULE_SIZE 60
#define BUTTON_MARGIN 20
#define BUTTON_HEIGHT 50
#define GRID_HEIGHT_RATIO 0.6 //60% de la hauteur de win
#define BUTTON_SIZE 60 

/* **************************************************************** */

Env * init(SDL_Window* win, SDL_Renderer* ren, int argc, char* argv[]);
void render(SDL_Window* win, SDL_Renderer* ren, Env * env);
void clean(SDL_Window* win, SDL_Renderer* ren, Env * env);
bool process(SDL_Window* win, SDL_Renderer* ren, Env * env, SDL_Event * e);

/* **************************************************************** */

#endif