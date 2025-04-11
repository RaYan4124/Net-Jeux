#include "game_tools.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"

bool test_game_load() {
  // Création d'un fichier temporaire
  const char *filename = "test_game.txt";
  FILE *file = fopen(filename, "w");

  if (!file) {
    fprintf(stderr, "Error: Unable to create test file.\n");
    return false;
  }

  // Écriture des données de test dans le fichier
  fprintf(file, "5 5 0\n");
  fprintf(file, "CW NN NW CN NS\n");
  fprintf(file, "TS TW TN TE TE\n");
  fprintf(file, "NE NN TW NW SE\n");
  fprintf(file, "NS TS TN CW SN\n");
  fprintf(file, "NE TW NS NE NS\n");
  fclose(file);

  game g = game_load((char *)filename);
  if (!g) {
    fprintf(stderr, "Error: game_load() returned NULL.\n");
    return false;
  }

  // Vérification
  assert(g->height == 5);
  assert(g->width == 5);
  assert(g->isWrapping == false);

  assert(g->cases[0].shape == CORNER && g->cases[0].orientation == WEST);
  assert(g->cases[1].shape == ENDPOINT && g->cases[1].orientation == NORTH);
  assert(g->cases[2].shape == ENDPOINT && g->cases[2].orientation == WEST);
  assert(g->cases[3].shape == CORNER && g->cases[3].orientation == NORTH);
  assert(g->cases[4].shape == ENDPOINT && g->cases[4].orientation == SOUTH);

  assert(g->cases[5].shape == TEE && g->cases[5].orientation == SOUTH);
  assert(g->cases[6].shape == TEE && g->cases[6].orientation == WEST);
  assert(g->cases[7].shape == TEE && g->cases[7].orientation == NORTH);
  assert(g->cases[8].shape == TEE && g->cases[8].orientation == EAST);
  assert(g->cases[9].shape == TEE && g->cases[9].orientation == EAST);

  assert(g->cases[10].shape == ENDPOINT && g->cases[10].orientation == EAST);
  assert(g->cases[11].shape == ENDPOINT && g->cases[11].orientation == NORTH);
  assert(g->cases[12].shape == TEE && g->cases[12].orientation == WEST);
  assert(g->cases[13].shape == ENDPOINT && g->cases[13].orientation == WEST);
  assert(g->cases[14].shape == SEGMENT && g->cases[14].orientation == EAST);

  assert(g->cases[15].shape == ENDPOINT && g->cases[15].orientation == SOUTH);
  assert(g->cases[16].shape == TEE && g->cases[16].orientation == SOUTH);
  assert(g->cases[17].shape == TEE && g->cases[17].orientation == NORTH);
  assert(g->cases[18].shape == CORNER && g->cases[18].orientation == WEST);
  assert(g->cases[19].shape == SEGMENT && g->cases[19].orientation == NORTH);

  assert(g->cases[20].shape == ENDPOINT && g->cases[20].orientation == EAST);
  assert(g->cases[21].shape == TEE && g->cases[21].orientation == WEST);
  assert(g->cases[22].shape == ENDPOINT && g->cases[22].orientation == SOUTH);
  assert(g->cases[23].shape == ENDPOINT && g->cases[23].orientation == EAST);
  assert(g->cases[24].shape == ENDPOINT && g->cases[24].orientation == SOUTH);

  game_delete(g);
  remove(filename);

  return true;
}

bool test_game_save() {
  game g = game_new_empty_ext(5, 5, false);
  g->cases[0].shape = CORNER;
  g->cases[0].orientation = WEST;
  g->cases[1].shape = ENDPOINT;
  g->cases[1].orientation = NORTH;
  g->cases[2].shape = ENDPOINT;
  g->cases[2].orientation = WEST;
  g->cases[3].shape = CORNER;
  g->cases[3].orientation = NORTH;
  g->cases[4].shape = ENDPOINT;
  g->cases[4].orientation = SOUTH;

  const char *filename = "test_save.txt";

  game_save(g, (char *)filename);

  // Vérification
  FILE *file = fopen(filename, "r");
  if (!file) {
    fprintf(stderr, "Error: Unable to open saved file.\n");
    return false;
  }

  int height, width, isWrapping;
  fscanf(file, "%d %d %d\n", &height, &width, &isWrapping);
  assert(height == 5);
  assert(width == 5);
  assert(isWrapping == 0);

  char shape, orientation;
  fscanf(file, "%c%c ", &shape, &orientation);
  assert(shape == 'C' && orientation == 'W');

  fclose(file);
  remove(filename);

  return true;
}

void usage(int argc, char *argv[]) {
  fprintf(stderr, "Usage: %s <testname> [<...>]\n", argv[0]);
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    usage(argc, argv);
  }

  bool etat = true;
  fprintf(stderr, "=> Start test \"%s\"\n", argv[1]);

  if (strcmp("game_load", argv[1]) == 0) {
    etat = test_game_load();
  } else if (strcmp("game_save", argv[1]) == 0) {
    etat = test_game_save();
  } else {
    fprintf(stderr, "Test \"%s\" finished: FAILURE\n", argv[1]);
    return EXIT_FAILURE;
  }

  // print test result
  if (etat) {
    fprintf(stderr, "Test \"%s\" finished: SUCCESS\n", argv[1]);
    return EXIT_SUCCESS;
  } else {
    fprintf(stderr, "Test \"%s\" finished: FAILURE\n", argv[1]);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}