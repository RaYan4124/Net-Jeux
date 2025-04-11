#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"
#include "game_tools.h"

int main(int argc, char *argv[]) {
  // Vérifier le nombre d'arguments
  if (argc < 7) {
    fprintf(stderr,
            "Usage: %s <nb_rows> <nb_cols> <wrapping> <nb_empty> <nb_extra> "
            "<shuffle> [<filename>]\n",
            argv[0]);
    return 1;
  }

  uint nb_rows = atoi(argv[1]);
  uint nb_cols = atoi(argv[2]);
  bool wrapping = atoi(argv[3]);
  uint nb_empty = atoi(argv[4]);
  uint nb_extra = atoi(argv[5]);
  bool shuffle = atoi(argv[6]);

  srand(time(NULL));

  game g = game_random(nb_rows, nb_cols, wrapping, nb_empty, nb_extra);
  if (!g) {
    fprintf(stderr, "Erreur dans la génération du jeu.\n");
    return 1;
  }

  if (shuffle) {
    game_shuffle_orientation(g);
  }
  printf("Copyright: Net Game by University of Bordeaux, 2024.\n");
  printf("nb_rows = %d nb_cols = %d wrapping = %d\n", nb_rows, nb_cols,
         wrapping);
  printf("nb_empty = %d extra = %d, shuffle = %d\n", nb_empty, nb_extra,
         shuffle);

  game_print(g);

  if (argc == 8) {
    const char *filename = argv[7];
    game_save(g, (char *)filename);
  }

  game_delete(g);

  return 0;
}
