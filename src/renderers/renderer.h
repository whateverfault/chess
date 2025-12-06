#ifndef CHESS_RENDERER_H
#define CHESS_RENDERER_H

#include "shared/game.h"

Turn get_turn(Piece field[FIELD_SIZE][FIELD_SIZE]);
void update(Piece field[FIELD_SIZE][FIELD_SIZE]);
void end();

#endif //CHESS_RENDERER_H
