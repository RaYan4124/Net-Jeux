#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"
#include "game_tools.h"

int main(int argc, char *argv[]) {
  game g;

  // Charger un fichier si un argument est passé, sinon charger le jeu par
  // défaut
  if (argc > 1) {
    g = game_load(argv[1]);
    if (!g) {
      fprintf(stderr, "Error: Unable to load game from file %s.\n", argv[1]);
      return EXIT_FAILURE;
    }
  } else {
    g = game_default();
  }

  game_print(g);

  while (!game_won(g)) {
    char player_choice;
    int i, j;
    char filename[100];

    printf("> ? [ h for help ]\n");
    scanf(" %c", &player_choice);

    if (player_choice == 'q') {
      printf("action: quit\nwhat a sham, you gave up :-(\n");
      break;
    } else if (player_choice == 'h') {
      printf(
          "> action: help\n - press 'c <i> <j>' to rotate piece clockwise in "
          "square (i,j)\n - press 'a <i> <j>' to rotate piece anti-clockwise "
          "in square (i,j)\n - press 'r' to shuffle game\n - press 'q' to "
          "quit\n"
          " - press 's <filename>' to save the current state of the game\n");
      game_print(g);

    } else if (player_choice == 'c') {
      scanf("%d %d", &i, &j);
      printf("action: play move 'c' into (%d,%d) square\n", i, j);
      game_play_move(g, i, j, 1);
      game_print(g);

    } else if (player_choice == 'a') {
      scanf("%d %d", &i, &j);
      printf("action: play move 'a' into (%d,%d) square\n", i, j);
      game_play_move(g, i, j, -1);
      game_print(g);

    } else if (player_choice == 'r') {
      printf("action: restart\n");
      game_shuffle_orientation(g);
      game_print(g);

    } else if (player_choice == 'z') {
      game_undo(g);
      game_print(g);

    } else if (player_choice == 'y') {
      game_redo(g);
      game_print(g);

    } else if (player_choice == 's') {
      scanf("%s", filename);
      game_save(g, filename);
      printf("Game successfully saved in %s\n", filename);
    }

    if (game_won(g)) {
      printf("Congratulations!!\n");
      game_delete(g);
      return EXIT_SUCCESS;
    }
  }

  game_delete(g);
  return EXIT_SUCCESS;
}
