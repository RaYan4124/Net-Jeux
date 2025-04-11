// SDL2 Demo by aurelien.esnard@u-bordeaux.fr

#include "model.h"

#include <SDL.h>
#include <SDL_image.h>  // required to load transparent texture from PNG
#include <SDL_ttf.h>    // required to use TTF fonts
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"
#include "game_tools.h"

#define CORNERT "res/CORNER.png"
#define ENDPOINTT "res/ENDPOINT.png"
#define SEGMENTT "res/SEGMENT.png"
#define TEET "res/TEE.png"
#define CROSST "res/CROSS.png"

#define NEXT "res/NEXT.png"
#define PREVIOUS "res/PREVIOUS.png"
#define QUIT "res/QUIT.png"
#define RESET "res/RESET.png"
#define SOLUTION "res/SOLUTION.png"
#define BACKGROUND "res/BACKGROUND.png"

#define FONT "res/SuperPeace.ttf"
/* **************************************************************** */

struct Env_t {
  game g;
  int game_state;

  SDL_Texture *piece_textures[5];
  SDL_Texture *button_textures[6];

  TTF_Font *font;
  SDL_Texture *textTexture;

  bool messageShown;
};

/* **************************************************************** */

Env *init(SDL_Window *win, SDL_Renderer *ren, int argc, char *argv[]) {
  Env *env = malloc(sizeof(struct Env_t));

  if (argc > 1) {
    env->g = game_load(argv[1]);
    if (!env->g) {
      fprintf(stderr, "Error: Unable to load game from file %s.\n", argv[1]);
      exit(EXIT_FAILURE);
    }
  } else {
    env->g = game_default();
  }

  // Charger les textures des pièces
  env->piece_textures[0] = IMG_LoadTexture(ren, ENDPOINTT);
  env->piece_textures[1] = IMG_LoadTexture(ren, SEGMENTT);
  env->piece_textures[2] = IMG_LoadTexture(ren, CORNERT);
  env->piece_textures[3] = IMG_LoadTexture(ren, TEET);
  env->piece_textures[4] = IMG_LoadTexture(ren, CROSST);

  // charger les textures des boutton
  env->button_textures[1] = IMG_LoadTexture(ren, NEXT);
  env->button_textures[0] = IMG_LoadTexture(ren, PREVIOUS);
  env->button_textures[4] = IMG_LoadTexture(ren, QUIT);
  env->button_textures[2] = IMG_LoadTexture(ren, RESET);
  env->button_textures[3] = IMG_LoadTexture(ren, SOLUTION);
  env->button_textures[5] = IMG_LoadTexture(ren, BACKGROUND);

  // Vérifier si les textures ont été bien chargées
  for (int i = 0; i < 5; i++) {
    if (!env->piece_textures[i]) {
      fprintf(stderr, "Erreur de chargement de la texture %d: %s\n", i,
              IMG_GetError());
      exit(EXIT_FAILURE);
    }
  }
  for (int i = 0; i < 6; i++) {
    if (!env->button_textures[i]) {
      fprintf(stderr, "Erreur de chargement de la texture %d: %s\n", i,
              IMG_GetError());
      exit(EXIT_FAILURE);
    }
  }

  // Charger la police
  env->font = TTF_OpenFont(FONT, 32);
  if (!env->font) {
    fprintf(stderr, "Erreur chargement police: %s\n", TTF_GetError());
    exit(EXIT_FAILURE);
  }

  SDL_Color textColor = {255, 255, 255, 255};

  // Créer la texture du texte
  SDL_Surface *textSurface =
      TTF_RenderText_Solid(env->font, "NET GAME", textColor);
  if (!textSurface) {
    fprintf(stderr, "Erreur rendu texte: %s\n", TTF_GetError());
    exit(EXIT_FAILURE);
  }

  env->textTexture = SDL_CreateTextureFromSurface(ren, textSurface);
  SDL_FreeSurface(textSurface);

  if (!env->textTexture) {
    fprintf(stderr, "Erreur texture texte: %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  env->game_state = 0;  // Jeu en cours
  env->messageShown = false;

  return env;
}

/* **************************************************************** */

void render(SDL_Window *win, SDL_Renderer *ren, Env *env) {
  // recuperation la taille actuelle de win
  int windowWidth, windowHeight;
  SDL_GetWindowSize(win, &windowWidth, &windowHeight);

  // Effacer l'écran avec une couleur de fond grise
  SDL_SetRenderDrawColor(ren, 0xA0, 0xA0, 0xA0, 0xFF);
  SDL_RenderClear(ren);
  if (env->button_textures[5]) {
    SDL_RenderCopy(ren, env->button_textures[5], NULL, NULL);
  }

  // Calcul de la taille et position de la grille
  int gridWidth = env->g->width * CELLULE_SIZE;
  int gridHeight = env->g->height * CELLULE_SIZE;

  int gridX = (windowWidth - gridWidth) / 2;
  int gridY = (windowHeight - gridHeight) / 2;

  // tailles des cellules
  int cellWidth = CELLULE_SIZE;
  int cellHeight = CELLULE_SIZE;

  // Obtenir les dimensions du texte
  int textWidth, textHeight;
  SDL_QueryTexture(env->textTexture, NULL, NULL, &textWidth, &textHeight);

  // Calculer la position centrée au-dessus de la grille
  int textX = (windowWidth - textWidth) / 2;
  int textY = gridY - textHeight - 10;  // Un peu au-dessus de la grille

  // Définir le rectangle de destination
  SDL_Rect textRect = {textX, textY, textWidth, textHeight};

  // Afficher le texte
  SDL_RenderCopy(ren, env->textTexture, NULL, &textRect);

  // Dessiner la grille avec les cellules
  SDL_SetRenderDrawColor(ren, 0, 255, 0, 255);
  for (int i = 0; i < env->g->height; i++) {
    for (int j = 0; j < env->g->width; j++) {
      SDL_Rect rect = {gridX + j * cellWidth, gridY + i * cellHeight, cellWidth,
                       cellHeight};
      if (env->game_state == 1) {
        SDL_SetRenderDrawColor(ren, 0, 255, 0, 255);  // Vert
      } else {
        SDL_SetRenderDrawColor(ren, 135, 206, 235,
                               255);  // Bleu ciel (Sky Blue)
      }

      SDL_RenderFillRect(ren, &rect);  // background blanc
      SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
      SDL_RenderDrawRect(ren, &rect);  // bordure noire

      shape pieceShape = env->g->cases[i * env->g->width + j]
                             .shape;  // recuperation de la shape de la cellule
      int pieceOrientation = env->g->cases[i * env->g->width + j]
                                 .orientation;  // recuperation de l'orientation

      // si la piece existe on dessine
      if (pieceShape != 0) {
        SDL_Texture *pieceTexture = env->piece_textures[pieceShape - 1];
        if (pieceTexture != NULL) {
          SDL_Rect pieceRect = rect;  // La pièce doit occuper toute la cellule
          double angle = (double)pieceOrientation * 90.0;
          SDL_Point center = {cellWidth / 2, cellHeight / 2};
          SDL_RenderCopyEx(ren, pieceTexture, NULL, &pieceRect, angle, &center,
                           SDL_FLIP_NONE);
        }
      }
    }
  }
  // Positionner les boutons en ligne sous la grille
  int totalButtonWidth = (5 * BUTTON_SIZE) +
                         (4 * BUTTON_MARGIN);  // espace qu'occupent les boutons
  int buttonX = (windowWidth - totalButtonWidth) / 2;  // centrage horizentale
  int buttonY =
      gridY + gridHeight +
      BUTTON_MARGIN;  // positionnement en dessous de la grille + marge

  SDL_Rect buttons[5];

  for (int i = 0; i < 5; i++) {
    buttons[i].x = buttonX + i * (BUTTON_SIZE + BUTTON_MARGIN);
    buttons[i].y = buttonY;
    buttons[i].w = BUTTON_SIZE;
    buttons[i].h = BUTTON_SIZE;

    // Afficher les textures des boutons si elles existent
    if (env->button_textures[i] != NULL) {
      SDL_RenderCopy(ren, env->button_textures[i], NULL, &buttons[i]);
    } else {
      // Dessiner un bouton gris par défaut si la texture est manquante
      SDL_SetRenderDrawColor(ren, 150, 150, 150, 255);
      SDL_RenderFillRect(ren, &buttons[i]);
      SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
      SDL_RenderDrawRect(ren, &buttons[i]);
    }
  }

  if (env->game_state == 1 && !env->messageShown) {
    SDL_ShowSimpleMessageBox(
        SDL_MESSAGEBOX_INFORMATION,             // Type de la boîte de message
                                                // (information)
        "Congratulation You Won !!!!",          // Titre de la boîte de message
        "You have successfully won the game!",  // Le message à afficher
        win                                     // Fenêtre associée
    );
    // Mettre à jour la variable pour éviter de montrer le message à nouveau
    env->messageShown = true;
  }

  SDL_RenderPresent(ren);
}

/* **************************************************************** */

bool process(SDL_Window *win, SDL_Renderer *ren, Env *env, SDL_Event *e) {
  if (e->type == SDL_QUIT) {
    return true;
  }

  // Récupération de la taille de la fenêtre
  int windowWidth, windowHeight;
  SDL_GetWindowSize(win, &windowWidth, &windowHeight);

  // Calcul de la position de la grille (même logique que le dessin)
  int gridWidth = env->g->width * CELLULE_SIZE;
  int gridHeight = env->g->height * CELLULE_SIZE;
  int gridX = (windowWidth - gridWidth) / 2;
  int gridY = (windowHeight - gridHeight) / 2;

  if (e->type == SDL_MOUSEBUTTONDOWN) {
    int x = e->button.x;
    int y = e->button.y;

    // Vérifier si le clic est dans la grille
    if (x >= gridX && x < gridX + gridWidth && y >= gridY &&
        y < gridY + gridHeight) {
      int j = (x - gridX) / CELLULE_SIZE;  // Colonne
      int i = (y - gridY) / CELLULE_SIZE;  // Ligne

      if (e->button.button == SDL_BUTTON_LEFT) {
        if (env->game_state == 0) {
          game_play_move(env->g, i, j, -1);
          if (game_won(env->g))
          {
            env->game_state = 1;
          }
          
        }
      } else if (e->button.button == SDL_BUTTON_RIGHT) {
        if (env->game_state == 0) {
          game_play_move(env->g, i, j, 1);
          if (game_won(env->g))
          {
            env->game_state = 1;
          }
        }
      }
    }

    // Position des boutons sous la grille
    int totalButtonWidth = (5 * BUTTON_SIZE) + (4 * BUTTON_MARGIN);
    int buttonX = (windowWidth - totalButtonWidth) / 2;
    int buttonY = gridY + gridHeight + BUTTON_MARGIN;

    SDL_Rect buttons[5];
    for (int i = 0; i < 5; i++) {
      buttons[i].x = buttonX + i * (BUTTON_SIZE + BUTTON_MARGIN);
      buttons[i].y = buttonY;
      buttons[i].w = BUTTON_SIZE;
      buttons[i].h = BUTTON_SIZE;
    }

    // Vérifier si le clic est sur l'un des boutons
    for (int i = 0; i < 5; i++) {
      if (x >= buttons[i].x && x <= buttons[i].x + BUTTON_SIZE &&
          y >= buttons[i].y && y <= buttons[i].y + BUTTON_SIZE) {
        switch (i) {
          case 0:
            if (env->game_state == 0) {
              printf("Annuler le coup\n");
              game_undo(env->g);
            }

            break;
          case 1:
            if (env->game_state == 0) {
              printf("Refaire le coup\n");
              game_redo(env->g);
            }

            break;
          case 2:
            printf("Réinitialiser la grille\n");
            game_shuffle_orientation(env->g);
            env->game_state = 0;
            break;
          case 3:
            printf("Solution du jeu\n");
            if (game_solve(env->g)) {
              env->game_state = 1;
            }
            break;
          case 4:
            printf("Quitter le jeu\n");
            env->game_state = 2;
            return true;
            break;
        }
      }
    }
  }

  // Gestion des raccourcis clavier
  if (e->type == SDL_KEYDOWN) {
    switch (e->key.keysym.sym) {
      case SDLK_z:
        if (env->game_state == 0) {
          printf("Annuler le coup\n");
          game_undo(env->g);
        }

        break;

      case SDLK_y:
        if (env->game_state == 0) {
          printf("Refaire le coup\n");
          game_redo(env->g);
        }
        break;

      case SDLK_r:
        printf("Réinitialiser la grille\n");
        game_shuffle_orientation(env->g);
        break;

      case SDLK_s:
        printf("Solution du jeu\n");
        if (game_solve(env->g)) {
          env->game_state = 1;
        }
        break;

      case SDLK_q:
        printf("Quitter le jeu\n");
        env->game_state = 2;
        return true;

      case SDLK_h:  // Si l'utilisateur appuie sur 'H', afficher la boîte de
                    // message
        printf("Affichage des règles du jeu\n");
        SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_INFORMATION, "Règles du jeu",
            "1. Le but du jeu est de résoudre la grille.\n"
            "2. Clic gauche de la souris pour faire tourner la pièce dans le "
            "sens contraire des aiguilles d'une montre.\n"
            "3. Clic droit de la souris pour faire tourner la pièce dans le "
            "sens des aiguilles d'une montre.\n"
            "4. Appuyez sur 'Z' pour annuler un coup et 'Y' pour refaire un "
            "coup.\n"
            "5. Appuyez sur 'R' pour réinitialiser la grille.\n"
            "6. Appuyez sur 'S' pour afficher la solution.\n"
            "7. Appuyez sur 'Q' pour quitter le jeu.",  // Le message à afficher
            win                                         // Fenêtre associée
        );
        break;
      default:
        break;
    }
  }

  return false;  // Continuer le jeu
}

/* **************************************************************** */

void clean(SDL_Window *win, SDL_Renderer *ren, Env *env) {
  for (int i = 0; i < 5; i++) {
    SDL_DestroyTexture(env->piece_textures[i]);
  }
  for (int i = 0; i < 6; i++) {
    SDL_DestroyTexture(env->button_textures[i]);
  }

  SDL_DestroyTexture(env->textTexture);
  TTF_CloseFont(env->font);

  free(env);
}

/* **************************************************************** */
