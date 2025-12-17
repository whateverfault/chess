#ifndef CHESS_RENDERER_H
#define CHESS_RENDERER_H

#include "shared/game.h"

Turn get_turn(GameContext *context);
PieceType get_promotion_option(Piece piece);

void init();
void update(GameContext *context);
void end();

void update_and_wait_for_input(GameContext *context);

#endif //CHESS_RENDERER_H
