#include <stdbool.h>

#define VEC_IMPLEMENTATION
#include "shared/vec.h"

#include "shared/game.h"
#include "renderers/renderer.h"

Piece make_piece(PieceType type, Side side){
    return (Piece){
            .type = type,
            .side = side,
    };
}

bool is_piece(Piece field[FIELD_SIZE][FIELD_SIZE], Vec2 pos, Side side){
    return (field[pos.y][pos.x].type != PIECE_NONE)
    && (side == SIDE_ANY || field[pos.y][pos.x].side == side);
}

bool king_can_move(Piece field[FIELD_SIZE][FIELD_SIZE], Turn turn){
    for (int y = -1; y <= 1; ++y){
        for (int x = -1; x <= 1; ++x){
             if (vec2_eq(turn.step, vec2_create(x, y))) {
                 return true;
             }
        }
    }
    
    return false;
}

bool bishop_can_move(Piece field[FIELD_SIZE][FIELD_SIZE], Turn turn){
    if (turn.step.x != turn.step.y
    && turn.step.x != -turn.step.y){
        return false;
    }

    int dir_x = turn.to.x > turn.from.x ? 1 : -1;
    int dir_y = turn.to.y > turn.from.y ? 1 : -1;

    if (dir_x > 0){
        if (dir_y > 0){
            int y = turn.from.y + 1;
            for (int x = turn.from.x + 1; x < turn.to.x; ++x, ++y) {
                if (!is_piece(field, vec2_create(x, y), SIDE_ANY)) continue;

                return false;
            }
        }else{
            int y = turn.from.y - 1;
            for (int x = turn.from.x + 1; x < turn.to.x; ++x, --y) {
                if (!is_piece(field, vec2_create(x, y), SIDE_ANY)) continue;

                return false;
            }
        }
    }else{
        if (dir_y > 0){
            int y = turn.from.y - 1;
            for (int x = turn.from.x - 1; x < turn.to.x; --x, ++y) {
                if (!is_piece(field, vec2_create(x, y), SIDE_ANY)) continue;

                return false;
            }
        }else{
            int y = turn.from.y - 1;
            for (int x = turn.from.x - 1; x < turn.to.x; --x, --y) {
                if (!is_piece(field, vec2_create(x, y), SIDE_ANY)) continue;

                return false;
            }
        }
    }
    
    return true;
}

bool rook_can_move(Piece field[FIELD_SIZE][FIELD_SIZE], Turn turn){
    if ((turn.step.x != 0 && turn.step.y != 0)){
        return false;
    }
    
    int dir_x = turn.to.x > turn.from.x ? 1 : -1;
    int dir_y = turn.to.y > turn.from.y ? 1 : -1;
    
    if (turn.step.x != 0){
        if (dir_x > 0){
            for (int x = turn.from.x + 1; x < turn.to.x; ++x){
                if (!is_piece(field, vec2_create(x, turn.from.y), SIDE_ANY)) continue;
                
                return false;
            }
        }else{
            for (int x = turn.to.x + 1; x < turn.from.x; ++x){
                if (!is_piece(field, vec2_create(x, turn.from.y), SIDE_ANY)) continue;

                return false;
            }
        }
    }

    if (turn.step.y != 0){
        if (dir_y > 0){
            for (int y = turn.from.y + 1; y < turn.to.y; ++y){
                if (!is_piece(field, vec2_create(turn.from.x, y), SIDE_ANY)) continue;

                return false;
            }
        }else{
            for (int y = turn.to.y + 1; y < turn.from.y; ++y){
                if (!is_piece(field, vec2_create(turn.from.x, y), SIDE_ANY)) continue;

                return false;
            }
        }
    }
    
    return true;
}

bool queen_can_move(Piece field[FIELD_SIZE][FIELD_SIZE], Turn turn){
    return rook_can_move(field, turn)
    || bishop_can_move(field, turn);
}

bool knight_can_move(Piece field[FIELD_SIZE][FIELD_SIZE], Turn turn){
    return vec2_eq(turn.step, vec2_create(2, 1))
           || vec2_eq(turn.step, vec2_create(-2, -1))
           || vec2_eq(turn.step, vec2_create(-2, 1))
           || vec2_eq(turn.step, vec2_create(2, -1))
           || vec2_eq(turn.step, vec2_create(2, 1))
           || vec2_eq(turn.step, vec2_create(-1, -2))
           || vec2_eq(turn.step, vec2_create(-1, 2))
           || vec2_eq(turn.step, vec2_create(1, -2));
}

bool pawn_can_move(Piece field[FIELD_SIZE][FIELD_SIZE], Turn turn){
    Piece piece = field[turn.from.y][turn.from.x];
    
    bool can_move = false;
    
    if (piece.side == SIDE_BLACK){
        if (!turn.eats){
            can_move |= vec2_eq(turn.step, vec2_create(0, 1));

            if (turn.from.y == 1){
                can_move |= vec2_eq(turn.step, vec2_create(0, 2));
            }
        }else{
            for (int x = -1; x <= 1; ++x){
                if (x == 0) continue;

                can_move |= vec2_eq(turn.step, vec2_create(x, 1));
            }
        }
    }else{
        if (!turn.eats){
            can_move |= vec2_eq(turn.step, vec2_create(0, -1));

            if (turn.from.y == FIELD_SIZE - 1 - 1){
                can_move |= vec2_eq(turn.step, vec2_create(0, -2));
            }
        }else{
            for (int x = -1; x <= 1; ++x){
                if (x == 0) continue;

                can_move |= vec2_eq(turn.step, vec2_create(x, -1));
            }
        }
    }
    
    return can_move;
}

bool can_move(Piece field[FIELD_SIZE][FIELD_SIZE], Turn turn) {
    if (vec2_eq(turn.from, turn.to)
    || !is_piece(field, turn.from, SIDE_ANY)){
        return false;
    }

    Piece piece = field[turn.from.y][turn.from.x];

    if (is_piece(field, turn.to, piece.side)){
        return false;
    }
    
    switch (piece.type) {
        
        case PIECE_KING: return king_can_move(field, turn);
        case PIECE_QUEEN: return queen_can_move(field, turn);
        case PIECE_ROOK: return rook_can_move(field, turn);
        case PIECE_BISHOP: return bishop_can_move(field, turn);
        case PIECE_KNIGHT: return knight_can_move(field, turn);
        case PIECE_PAWN: return pawn_can_move(field, turn);
        
        default: return false;
    }
}

void apply_turn(Piece field[FIELD_SIZE][FIELD_SIZE], Turn turn){
    if (can_move(field, turn)){
        field[turn.to.y][turn.to.x] = field[turn.from.y][turn.from.x];
        field[turn.from.y][turn.from.x] = make_piece(PIECE_NONE, SIDE_ANY);
    }
}

int main(void) {
    Piece field[FIELD_SIZE][FIELD_SIZE] = {
            make_piece(PIECE_ROOK, SIDE_BLACK), make_piece(PIECE_KNIGHT, SIDE_BLACK), make_piece(PIECE_BISHOP, SIDE_BLACK), make_piece(PIECE_QUEEN, SIDE_BLACK), make_piece(PIECE_KING, SIDE_BLACK), make_piece(PIECE_BISHOP, SIDE_BLACK), make_piece(PIECE_KNIGHT, SIDE_BLACK), make_piece(PIECE_ROOK, SIDE_BLACK),
            make_piece(PIECE_PAWN, SIDE_BLACK), make_piece(PIECE_PAWN, SIDE_BLACK), make_piece(PIECE_PAWN, SIDE_BLACK), make_piece(PIECE_PAWN, SIDE_BLACK), make_piece(PIECE_PAWN, SIDE_BLACK), make_piece(PIECE_PAWN, SIDE_BLACK), make_piece(PIECE_PAWN, SIDE_BLACK), make_piece(PIECE_PAWN, SIDE_BLACK),
            make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY),
            make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY),
            make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY),
            make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY),
            make_piece(PIECE_PAWN, SIDE_WHITE), make_piece(PIECE_PAWN, SIDE_WHITE), make_piece(PIECE_PAWN, SIDE_WHITE), make_piece(PIECE_PAWN, SIDE_WHITE), make_piece(PIECE_PAWN, SIDE_WHITE), make_piece(PIECE_PAWN, SIDE_WHITE), make_piece(PIECE_PAWN, SIDE_WHITE), make_piece(PIECE_PAWN, SIDE_WHITE),
            make_piece(PIECE_ROOK, SIDE_WHITE), make_piece(PIECE_KNIGHT, SIDE_WHITE), make_piece(PIECE_BISHOP, SIDE_WHITE), make_piece(PIECE_QUEEN, SIDE_WHITE), make_piece(PIECE_KING, SIDE_WHITE), make_piece(PIECE_BISHOP, SIDE_WHITE), make_piece(PIECE_KNIGHT, SIDE_WHITE), make_piece(PIECE_ROOK, SIDE_WHITE),
    };
    
    while (true){
        update(field);
        
        apply_turn(field, get_turn(field));
        
        end();
    }
}

/*
    TODO: Implement swapping the turning side after the turn
    TODO: Implement pawn promotion
    TODO: Implement checks and mates
    TODO: Implement castling and en passant 
*/