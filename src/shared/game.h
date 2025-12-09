#ifndef CHESS_GAME_H
#define CHESS_GAME_H

#include <stdbool.h>

#include "shared/vec.h"

#define FIELD_SIZE 8

typedef enum {
    PIECE_NONE,
    PIECE_KING,
    PIECE_QUEEN,
    PIECE_KNIGHT,
    PIECE_BISHOP,
    PIECE_ROOK,
    PIECE_PAWN,
} PieceType;

typedef enum {
    SIDE_ANY,
    SIDE_BLACK,
    SIDE_WHITE,
} Side;

typedef struct {
    PieceType type;
    Side side;
} Piece;

typedef struct {
    Vec2 from;
    Vec2 to;
    Vec2 step;
    
    bool eats;
    bool valid;
} Turn;

typedef struct {
    Side turning_side;
    Piece field[FIELD_SIZE][FIELD_SIZE];
} GameContext;

bool can_move(GameContext *context, Turn turn);
Piece make_piece(PieceType type, Side side);
Side opposite_side(Side side);
bool is_piece(GameContext *context, Vec2 pos, Side side);

#endif //CHESS_GAME_H
