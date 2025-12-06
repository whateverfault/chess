#include <stdio.h>

#include "renderer.h"

#define NOTHING_IMPLEMENTATION
#include "nothing/nothing.h"

static String_Builder buffer = {0};

#define TURN_BUFFER_LENGTH 5

int ascii_num_to_num(char ascii){
    if (ascii >= '0' && ascii <= '9'){
        return ascii - 48;
    }

    return -1;
}

int ascii_alpha_to_num(char ascii){
    if (ascii >= 'A' && ascii <= 'Z'){
        return ascii - 65;
    }

    if (ascii >= 'a' && ascii <= 'z'){
        return ascii - 97;
    }

    return -1;
}

bool is_valid_input(char* turn_buffer, size_t len){
    if (len < 4) return false;

    bool valid = true;

    int from_x = ascii_alpha_to_num(turn_buffer[0]);
    int from_y = ascii_num_to_num(turn_buffer[1]) - 1;
    int to_x = ascii_alpha_to_num(turn_buffer[2]);
    int to_y = ascii_num_to_num(turn_buffer[3]) - 1;

    valid &= (from_x >= 0 && from_x < FIELD_SIZE);
    valid &= (from_y >= 0 && from_y < FIELD_SIZE);
    valid &= (to_x >= 0 && to_x < FIELD_SIZE);
    valid &= (to_y >= 0 && to_y < FIELD_SIZE);

    return valid;
}

Side opposite_side(Side side){
    if (side == SIDE_WHITE){
        return SIDE_BLACK;
    }else if(side == SIDE_BLACK){
        return SIDE_WHITE;
    }

    return side;
}

Turn get_turn(Piece field[FIELD_SIZE][FIELD_SIZE]){
    Turn turn = {0};
    bool valid = false;

    char turn_buffer[TURN_BUFFER_LENGTH] = {0};

    while (!valid) {
        fgets(turn_buffer, sizeof(turn_buffer), stdin);

        valid = is_valid_input(turn_buffer, TURN_BUFFER_LENGTH);
    }

    turn.from = (Vec2){
            .x = ascii_alpha_to_num(turn_buffer[0]),
            .y = FIELD_SIZE - ascii_num_to_num(turn_buffer[1]),
    };

    turn.to = (Vec2){
            .x = ascii_alpha_to_num(turn_buffer[2]),
            .y = FIELD_SIZE - ascii_num_to_num(turn_buffer[3]),
    };

    turn.step = vec2_subtract(turn.to, turn.from);

    turn.eats = (field[turn.to.y][turn.to.x].side == opposite_side(field[turn.from.y][turn.from.x].side));

    return turn;
}

static void draw_piece(String_Builder *sb, Piece piece){
    switch (piece.type) {
        case PIECE_KING:{
            sb_appendf(sb, "KI");
        } break;

        case PIECE_QUEEN:{
            sb_appendf(sb, "QU");
        } break;

        case PIECE_ROOK:{
            sb_appendf(sb, "RO");
        } break;

        case PIECE_KNIGHT:{
            sb_appendf(sb, "KN");
        } break;

        case PIECE_BISHOP:{
            sb_appendf(sb, "BI");
        } break;

        case PIECE_PAWN:{
            sb_appendf(sb, "PA");
        } break;
        
        case PIECE_NONE:{
            sb_appendf(sb, "##");
        } break;
    }
}

void update(Piece field[FIELD_SIZE][FIELD_SIZE]){
    String_View line = {0};
    
    buffer.count = 0;
    size_t line_number = 0;
    
    for (size_t x = 0; x < FIELD_SIZE; ++x) {
        
        sb_appendf(&buffer, "%d | ", 8 - x);
        
        for (size_t y = 0; y < FIELD_SIZE; ++y) {
            draw_piece(&buffer, field[x][y]);
            
            if (y < FIELD_SIZE - 1){
                sb_appendf(&buffer, " | ");
            }
        }

        sb_appendf(&buffer, " |");
        
        line_number += line_number % 2;
        
        sb_get_line(&buffer, &line, line_number++);

        sb_appendf(&buffer, "\n  ");
        for (int i = 0; i < line.count - 2; ++i) {
            sb_appendc(&buffer, '-');
        }
        sb_appendc(&buffer, '\n');
    }

    sb_appendf(&buffer, "    ");
    
    for (size_t x = 0; x < FIELD_SIZE; ++x) {
        sb_appendf(&buffer, "%c    ", x + 65);
    }

    sb_get_line(&buffer, &line, 0);

    printf("  ");
    for (int i = 0; i < line.count - 2; ++i) {
        printf("-");
    }
    printf("\n");
    
    printf("%s\n", buffer.items);
}

void end(){
    printf("\x1b[2J\x1b[3J\x1b[H");
}