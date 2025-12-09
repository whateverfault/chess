#include <stdbool.h>

#define VEC_IMPLEMENTATION
#include "shared/vec.h"

#include "shared/game.h"
#include "renderers/renderer.h"

Side opposite_side(Side side){
    if (side == SIDE_WHITE) return SIDE_BLACK;
    if (side == SIDE_BLACK) return SIDE_WHITE;
    
    return side;
}

Piece make_piece(PieceType type, Side side){
    return (Piece){
            .type = type,
            .side = side,
    };
}

bool is_piece(GameContext *context, Vec2 pos, Side side){
    return (context->field[pos.y][pos.x].type != PIECE_NONE)
    && (side == SIDE_ANY || context->field[pos.y][pos.x].side == side);
}

bool king_can_move(GameContext *context, Turn turn){
    for (int y = -1; y <= 1; ++y){
        for (int x = -1; x <= 1; ++x){
             if (vec2_eq(turn.step, vec2_create(x, y))) {
                 return true;
             }
        }
    }
    
    return false;
}

bool bishop_can_move(GameContext *context, Turn turn){
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
                if (!is_piece(context, vec2_create(x, y), SIDE_ANY)) continue;

                return false;
            }
        }else{
            int y = turn.from.y - 1;
            for (int x = turn.from.x + 1; x < turn.to.x; ++x, --y) {
                if (!is_piece(context, vec2_create(x, y), SIDE_ANY)) continue;

                return false;
            }
        }
    }else{
        if (dir_y > 0){
            int y = turn.from.y - 1;
            for (int x = turn.from.x - 1; x < turn.to.x; --x, ++y) {
                if (!is_piece(context, vec2_create(x, y), SIDE_ANY)) continue;

                return false;
            }
        }else{
            int y = turn.from.y - 1;
            for (int x = turn.from.x - 1; x < turn.to.x; --x, --y) {
                if (!is_piece(context, vec2_create(x, y), SIDE_ANY)) continue;

                return false;
            }
        }
    }
    
    return true;
}

bool rook_can_move(GameContext *context, Turn turn){
    if ((turn.step.x != 0 && turn.step.y != 0)){
        return false;
    }
    
    int dir_x = turn.to.x > turn.from.x ? 1 : -1;
    int dir_y = turn.to.y > turn.from.y ? 1 : -1;
    
    if (turn.step.x != 0){
        if (dir_x > 0){
            for (int x = turn.from.x + 1; x < turn.to.x; ++x){
                if (!is_piece(context, vec2_create(x, turn.from.y), SIDE_ANY)) continue;
                
                return false;
            }
        }else{
            for (int x = turn.to.x + 1; x < turn.from.x; ++x){
                if (!is_piece(context, vec2_create(x, turn.from.y), SIDE_ANY)) continue;

                return false;
            }
        }
    }

    if (turn.step.y != 0){
        if (dir_y > 0){
            for (int y = turn.from.y + 1; y < turn.to.y; ++y){
                if (!is_piece(context, vec2_create(turn.from.x, y), SIDE_ANY)) continue;

                return false;
            }
        }else{
            for (int y = turn.to.y + 1; y < turn.from.y; ++y){
                if (!is_piece(context, vec2_create(turn.from.x, y), SIDE_ANY)) continue;

                return false;
            }
        }
    }
    
    return true;
}

bool queen_can_move(GameContext *context, Turn turn){
    return rook_can_move(context, turn)
    || bishop_can_move(context, turn);
}

bool knight_can_move(GameContext *context, Turn turn){
    return vec2_eq(turn.step, vec2_create(2, 1))
           || vec2_eq(turn.step, vec2_create(-2, -1))
           || vec2_eq(turn.step, vec2_create(-2, 1))
           || vec2_eq(turn.step, vec2_create(2, -1))
           || vec2_eq(turn.step, vec2_create(2, 1))
           || vec2_eq(turn.step, vec2_create(-1, -2))
           || vec2_eq(turn.step, vec2_create(-1, 2))
           || vec2_eq(turn.step, vec2_create(1, -2));
}

bool pawn_can_move(GameContext *context, Turn turn){
    Piece piece = context->field[turn.from.y][turn.from.x];
    
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

bool can_move(GameContext *context, Turn turn) {
    if (vec2_eq(turn.from, turn.to)
    || !is_piece(context, turn.from, SIDE_ANY)){
        return false;
    }

    Piece piece = context->field[turn.from.y][turn.from.x];
    
    if (piece.side != context->turning_side){
        return false;
    }
    
    if (is_piece(context, turn.to, piece.side)){
        return false;
    }
    
    switch (piece.type) {
        
        case PIECE_KING: return king_can_move(context, turn);
        case PIECE_QUEEN: return queen_can_move(context, turn);
        case PIECE_ROOK: return rook_can_move(context, turn);
        case PIECE_BISHOP: return bishop_can_move(context, turn);
        case PIECE_KNIGHT: return knight_can_move(context, turn);
        case PIECE_PAWN: return pawn_can_move(context, turn);
        
        default: return false;
    }
}

bool apply_pawn_promotion(GameContext *context, Turn turn){
    Piece *piece = &context->field[turn.to.y][turn.to.x];
    
    PieceType promotion_option = get_promotion_option(*piece);
    
    if (promotion_option == PIECE_NONE){
        promotion_option = PIECE_QUEEN;
    }

    piece->type = promotion_option;
    
    return true;
}

bool apply_pawn_extra(GameContext *context, Turn turn){
    Piece piece = context->field[turn.to.y][turn.to.x];

    switch (piece.side) {
        
        case SIDE_WHITE:{
            if (turn.to.y == 0){
                return apply_pawn_promotion(context, turn);
            }
        } return false;

        case SIDE_BLACK:{
            if (turn.to.y == FIELD_SIZE - 1){
                return apply_pawn_promotion(context, turn);
            }
        } return false;
        
        
        default: return false;
    }
}

bool apply_extra(GameContext *context, Turn turn){
    Piece piece = context->field[turn.to.y][turn.to.x];

    switch (piece.type) {
        case PIECE_PAWN: return apply_pawn_extra(context, turn);
        default: return false;
    }
}

bool apply_turn(GameContext *context, Turn turn){
    bool valid_turn = (turn.valid && can_move(context, turn));
    
    if (!valid_turn) {
        return false;
    }
    
    context->field[turn.to.y][turn.to.x] = context->field[turn.from.y][turn.from.x];
    context->field[turn.from.y][turn.from.x] = make_piece(PIECE_NONE, SIDE_ANY);
    
    return true;
}

int main(void) {
    GameContext context = {
            .turning_side = SIDE_WHITE,
            .field =
            make_piece(PIECE_NONE, SIDE_BLACK), make_piece(PIECE_KNIGHT, SIDE_BLACK), make_piece(PIECE_BISHOP, SIDE_BLACK), make_piece(PIECE_QUEEN, SIDE_BLACK), make_piece(PIECE_KING, SIDE_BLACK), make_piece(PIECE_BISHOP, SIDE_BLACK), make_piece(PIECE_KNIGHT, SIDE_BLACK), make_piece(PIECE_ROOK, SIDE_BLACK),
            make_piece(PIECE_PAWN, SIDE_WHITE), make_piece(PIECE_PAWN, SIDE_BLACK), make_piece(PIECE_PAWN, SIDE_BLACK), make_piece(PIECE_PAWN, SIDE_BLACK), make_piece(PIECE_PAWN, SIDE_BLACK), make_piece(PIECE_PAWN, SIDE_BLACK), make_piece(PIECE_PAWN, SIDE_BLACK), make_piece(PIECE_PAWN, SIDE_BLACK),
            make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY),
            make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY),
            make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY),
            make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY), make_piece(PIECE_NONE, SIDE_ANY),
            make_piece(PIECE_PAWN, SIDE_WHITE), make_piece(PIECE_PAWN, SIDE_BLACK), make_piece(PIECE_PAWN, SIDE_WHITE), make_piece(PIECE_PAWN, SIDE_WHITE), make_piece(PIECE_PAWN, SIDE_WHITE), make_piece(PIECE_PAWN, SIDE_WHITE), make_piece(PIECE_PAWN, SIDE_WHITE), make_piece(PIECE_PAWN, SIDE_WHITE),
            make_piece(PIECE_ROOK, SIDE_WHITE), make_piece(PIECE_NONE, SIDE_WHITE), make_piece(PIECE_BISHOP, SIDE_WHITE), make_piece(PIECE_QUEEN, SIDE_WHITE), make_piece(PIECE_KING, SIDE_WHITE), make_piece(PIECE_BISHOP, SIDE_WHITE), make_piece(PIECE_KNIGHT, SIDE_WHITE), make_piece(PIECE_ROOK, SIDE_WHITE),
    };
    
    Turn previous_turn = {
            .valid = false,
    };
    
    init();
    
    bool applied = false;
    
    while (true){
        update(&context);

        if (previous_turn.valid){
            applied = apply_extra(&context, previous_turn);

            if (applied){
                end();
                continue;
            }
        }
        
        Turn turn = get_turn(&context);
        
        applied = apply_turn(&context, turn);
        
        end();

        if (!applied){
            continue;
        }

        previous_turn = turn;
        context.turning_side = opposite_side(context.turning_side);
    }
}

/*
    TODO: Implement checks and mates
    TODO: Implement castling and en passant 
*/