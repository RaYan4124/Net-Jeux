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

// @copyright University of Bordeaux. All rights reserved, 2024.

/* ************************************************************************** */

/** @brief Hard-coding of pieces (shape & orientation) in an integer array.
 * @details The 4 least significant bits encode the presence of an half-edge in
 * the N-E-S-W directions (in that order). Thus, binary coding 1100 represents
 * the piece "└" (a corner in north orientation).
 */
static uint _code[NB_SHAPES][NB_DIRS] = {
    {0b0000, 0b0000, 0b0000, 0b0000},  // EMPTY {" ", " ", " ", " "}
    {0b1000, 0b0100, 0b0010, 0b0001},  // ENDPOINT {"^", ">", "v", "<"},
    {0b1010, 0b0101, 0b1010, 0b0101},  // SEGMENT {"|", "-", "|", "-"},
    {0b1100, 0b0110, 0b0011, 0b1001},  // CORNER {"└", "┌", "┐", "┘"}
    {0b1101, 0b1110, 0b0111, 0b1011},  // TEE {"┴", "├", "┬", "┤"}
    {0b1111, 0b1111, 0b1111, 0b1111}   // CROSS {"+", "+", "+", "+"}
};

/* ************************************************************************** */

/** encode a shape and an orientation into an integer code */
static uint _encode_shape(shape s, direction o) { return _code[s][o]; }

/* ************************************************************************** */

/** decode an integer code into a shape and an orientation */
static bool _decode_shape(uint code, shape *s, direction *o) {
  assert(code >= 0 && code < 16);
  assert(s);
  assert(o);
  for (int i = 0; i < NB_SHAPES; i++)
    for (int j = 0; j < NB_DIRS; j++)
      if (code == _code[i][j]) {
        *s = i;
        *o = j;
        return true;
      }
  return false;
}

/* ************************************************************************** */

/** add an half-edge in the direction d */
static void _add_half_edge(game g, uint i, uint j, direction d) {
  assert(g);
  assert(i < game_nb_rows(g));
  assert(j < game_nb_cols(g));
  assert(d < NB_DIRS);

  shape s = game_get_piece_shape(g, i, j);
  direction o = game_get_piece_orientation(g, i, j);
  uint code = _encode_shape(s, o);
  uint mask = 0b1000 >> d;     // mask with half-edge in the direction d
  assert((code & mask) == 0);  // check there is no half-edge in the direction d
  uint newcode = code | mask;  // add the half-edge in the direction d
  shape news;
  direction newo;
  bool ok = _decode_shape(newcode, &news, &newo);
  assert(ok);
  game_set_piece_shape(g, i, j, news);
  game_set_piece_orientation(g, i, j, newo);
}

/* ************************************************************************** */

#define OPPOSITE_DIR(d) ((d + 2) % NB_DIRS)

/* ************************************************************************** */

/**
 * @brief Add an edge between two adjacent squares.
 * @details This is done by modifying the pieces of the two adjacent squares.
 * More precisely, we add an half-edge to each adjacent square, so as to build
 * an edge between these two squares.
 * @param g the game
 * @param i row index
 * @param j column index
 * @param d the direction of the adjacent square
 * @pre @p g must be a valid pointer toward a game structure.
 * @pre @p i < game height
 * @pre @p j < game width
 * @return true if an edge can be added, false otherwise
 */
static bool _add_edge(game g, uint i, uint j, direction d) {
  assert(g);
  assert(i < game_nb_rows(g));
  assert(j < game_nb_cols(g));
  assert(d < NB_DIRS);

  uint nexti, nextj;
  bool next = game_get_ajacent_square(g, i, j, d, &nexti, &nextj);
  if (!next) return false;

  // check if the two half-edges are free
  bool he = game_has_half_edge(g, i, j, d);
  if (he) return false;
  bool next_he = game_has_half_edge(g, nexti, nextj, OPPOSITE_DIR(d));
  if (next_he) return false;

  _add_half_edge(g, i, j, d);
  _add_half_edge(g, nexti, nextj, OPPOSITE_DIR(d));

  return true;
}

/* ************************************************************************** */

game game_load(char *filename) {
  if (filename == NULL) {
    fprintf(stderr, "Error: NULL pointer detected.\n");
    exit(EXIT_FAILURE);
  }

  FILE *file = fopen(filename, "r");

  if (file == NULL) {
    fprintf(stderr, "Error: NULL pointer detected.\n");
    exit(EXIT_FAILURE);
  }

  uint height, width, wrapp;
  bool isWrapping;

  fscanf(file, "%d %d %d\n", &height, &width, &wrapp);

  isWrapping = (wrapp == 0) ? false : true;

  game g = game_new_empty_ext(height, width, isWrapping);
  if (g == NULL) {
    fprintf(stderr, "Error : NULL pointer detected.\n");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      char shape, orientation;
      fscanf(file, "%c%c ", &shape, &orientation);

      switch (shape) {
        case 'E':
          g->squares[i * g->nb_cols + j].s = EMPTY;
          break;
        case 'N':
          g->squares[i * g->nb_cols + j].s = ENDPOINT;
          break;
        case 'S':
          g->squares[i * g->nb_cols + j].s = SEGMENT;
          break;
        case 'C':
          g->squares[i * g->nb_cols + j].s = CORNER;
          break;
        case 'T':
          g->squares[i * g->nb_cols + j].s = TEE;
          break;
        case 'X':
          g->squares[i * g->nb_cols + j].s = CROSS;
          break;
        default:
          fprintf(stderr, "Error: Invalid shape detected.\n");
          break;
      }

      switch (orientation) {
        case 'N':
          g->squares[i * g->nb_cols + j].o = NORTH;
          break;
        case 'S':
          g->squares[i * g->nb_cols + j].o = SOUTH;
          break;
        case 'E':
          g->squares[i * g->nb_cols + j].o = EAST;
          break;
        case 'W':
          g->squares[i * g->nb_cols + j].o = WEST;
          break;
        default:
          fprintf(stderr, "Error: Invalid orientation detected.\n");
          break;
      }
    }
    fscanf(file, "\n");
  }
  fclose(file);
  return g;
}

void game_save(cgame g, char *filename) {
  if (g == NULL) {
    fprintf(stderr, "Error: NULL pointer detected.\n");
    exit(EXIT_FAILURE);
  }

  if (filename == NULL) {
    fprintf(stderr, "Error: NULL pointer detected.\n");
    exit(EXIT_FAILURE);
  }

  FILE *file = fopen(filename, "w");

  if (file == NULL) {
    fprintf(stderr, "Error: NULL pointer detected.\n");
    exit(EXIT_FAILURE);
  }

  fprintf(file, "%d %d %d\n", g->nb_rows, g->nb_cols, g->wrapping);

  for (int i = 0; i < g->nb_rows; i++) {
    for (int j = 0; j < g->nb_cols; j++) {
      switch (g->squares[i * g->nb_cols + j].s) {
        case EMPTY:
          fprintf(file, "E");
          break;
        case ENDPOINT:
          fprintf(file, "N");
          break;
        case SEGMENT:
          fprintf(file, "S");
          break;
        case CORNER:
          fprintf(file, "C");
          break;
        case TEE:
          fprintf(file, "T");
          break;
        case CROSS:
          fprintf(file, "X");
          break;
        default:
          fprintf(stderr, "Error: Invalid shape detected.\n");
          break;
      }

      switch (g->squares[i * g->nb_cols + j].o) {
        case NORTH:
          fprintf(file, "N");
          break;
        case SOUTH:
          fprintf(file, "S");
          break;
        case EAST:
          fprintf(file, "E");
          break;
        case WEST:
          fprintf(file, "W");
          break;
        default:
          fprintf(stderr, "Error: Invalid orientation detected.\n");
          break;
      }

      fprintf(file, " ");
    }
    fprintf(file, "\n");
  }
  fclose(file);
}

game game_random(uint nb_rows, uint nb_cols, bool wrapping, uint nb_empty,
                 uint nb_extra) {
  if (nb_cols * nb_rows < 2 || nb_empty > nb_cols * nb_rows - 2) {
    return NULL;
  }

  game g = game_new_empty_ext(nb_rows, nb_cols, wrapping);
  if (!g) {
    return NULL;
  }

  uint i1 = rand() % nb_rows;
  uint j1 = rand() % nb_cols;
  direction d = rand() % NB_DIRS;

  do {
    // Placer un jeu solution à 2 pièces (soit horizontalement, soit
    // verticalement)
    i1 = rand() % nb_rows;
    j1 = rand() % nb_cols;
    d = rand() % NB_DIRS;

  } while (!_add_edge(g, i1, j1, d));  // les deux premières pièces

  int nbPieces = nb_rows * nb_cols - nb_empty - 2;
  uint i2, j2;

  while (nbPieces > 0) {
    i1 = rand() % nb_rows;
    j1 = rand() % nb_cols;
    d = rand() % NB_DIRS;

    if (game_get_piece_shape(g, i1, j1) != EMPTY &&
        game_get_ajacent_square(g, i1, j1, d, &i2, &j2) &&
        game_get_piece_shape(g, i2, j2) == EMPTY) {
      if (_add_edge(g, i1, j1, d)) {
        nbPieces--;
      }
    }
  }

  while (nb_extra > 0) {
    i1 = rand() % nb_rows;
    j1 = rand() % nb_cols;
    d = rand() % NB_DIRS;

    if (game_get_piece_shape(g, i1, j1) != EMPTY &&
        game_get_ajacent_square(g, i1, j1, d, &i2, &j2) &&
        game_get_piece_shape(g, i2, j2) != EMPTY &&
        game_check_edge(g, i1, j1, d) != MATCH) {
      if (_add_edge(g, i1, j1, d)) {
        nb_extra--;
      }
    }
  }

  return g;
}

void recSolve(game g, uint index, uint *count, bool trouverPremier) {
  if (game_won(g)) {
    (*count)++;
    if (trouverPremier)
      return;  // Arrêter dès qu'on trouve une solution choisi par l'utilisateur
  }

  if (index == g->nb_cols * g->nb_rows) {
    return;
  }

  direction original_orientation = g->squares[index].o;

  // Liste des orientations possibles en fonction de la forme de la pièce
  direction orientations[4] = {NORTH, EAST, SOUTH, WEST};
  int num_orientations = (g->squares[index].s == SEGMENT) ? 2
                         : (g->squares[index].s == CROSS) ? 1
                                                            : 4;

  // Essayer toutes les orientations possibles
  for (int i = 0; i < num_orientations; i++) {
    g->squares[index].o = orientations[i];
    if (game_check_edge(g, index / game_nb_rows(g), index % game_nb_cols(g),
                        NORTH) != MISMATCH &&
        game_check_edge(g, index / game_nb_rows(g), index % game_nb_cols(g),
                        WEST) != MISMATCH) {
      recSolve(g, index + 1, count, trouverPremier);
    }

    if (trouverPremier && *count > 0)
      return;  // Sortir dès qu'on trouve une solution
  }
  //  Restaurer l'orientation initiale avant de revenir
  g->squares[index].o = original_orientation;
}

bool game_solve(game g) {
  uint count = 0;
  recSolve(g, 0, &count, true);
  if (count == 0) return false;

  return true;
}

uint game_nb_solutions(cgame g) {
  uint count = 0;

  recSolve(g, 0, &count, false);

  return count;
}