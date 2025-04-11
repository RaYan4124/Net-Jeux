/**
 * @file game_struct.h
 * @brief Basic Game Functions.
 * @details See @ref index for further details.
 * @copyright University of Bordeaux. All rights reserved, 2024.
 **/

#ifndef __GAME_STRUCT__H__
#define __GAME_STRUCT__H__

#include <stdbool.h>
#include "game.h"
#include "game_aux.h"
#include "queue.h"



typedef struct{
    direction orientation;
    shape shape;
} Acase;

struct game_s{
    uint height;
    uint width;
    Acase* cases;
    bool isWrapping;
    queue* do_queue;
    queue* undo_queue;
    //previous_move previous;
};





             
#endif