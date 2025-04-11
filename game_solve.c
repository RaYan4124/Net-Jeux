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
  if (argc < 3 || argc > 4) {
    fprintf(stderr, "Usage: %s <option> <input> [<output>]\n", argv[0]);
    return EXIT_FAILURE;
  }

  char *option = argv[1];
  char *input_filename = argv[2];
  char *output_filename = (argc == 4) ? argv[3] : NULL;

  // Chargement du jeu
  game g = game_load(input_filename);
  if (!g) {
    fprintf(stderr, "Erreur : impossible de charger le jeu depuis %s\n",
            input_filename);
    return EXIT_FAILURE;
  }
  // game_print(g);

  bool solve = false;
  uint nbSolutions = 0;
  // Traitement des options
  if (strcmp(option, "-s") == 0) {
    solve = game_solve(g);
    if (!solve) {
      fprintf(stderr, "Aucune solution trouvée.\n");
      game_delete(g);
      return EXIT_FAILURE;
    }

  } else if (strcmp(option, "-c") == 0) {
    nbSolutions = game_nb_solutions(g);
  } else {
    fprintf(stderr, "Option invalide : %s\n", option);
    game_delete(g);
    return EXIT_FAILURE;
  }

  // Écriture du résultat dans le fichier de sortie ou affichage
  if (output_filename) {
    FILE *output_file = fopen(output_filename, "w");
    if (!output_file) {
      fprintf(stderr, "Erreur : impossible d'écrire dans %s\n",
              output_filename);
      game_delete(g);
      return EXIT_FAILURE;
    }
    if (strcmp(option, "-s") == 0) {
      game_save(g, (char *)output_file);  // Sauvegarde de la solution
    } else {
      fprintf(output_file, "%u\n",
              nbSolutions);  // Écriture du nombre de solutions
    }
    fclose(output_file);
  } else {
    if (strcmp(option, "-s") == 0) {
      game_print(g);
    } else {
      printf("Nombre de solution : %u\n", nbSolutions);
    }
  }

  // Libération de la mémoire
  game_delete(g);
  return 0;
}
