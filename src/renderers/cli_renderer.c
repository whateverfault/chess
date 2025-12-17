#include <stdio.h>
#include <locale.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include "renderer.h"

#define NOTHING_IMPLEMENTATION
#include "nothing/nothing.h"

#define CONSOLE_IMPLEMENTATION
#include "shared/console.h"

static String_Builder buffer = {0};

#define TURN_BUFFER_LENGTH 5

void clear_stdin(){
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

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

bool is_valid_turn_input(char turn_buffer[TURN_BUFFER_LENGTH]){
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

Turn get_turn(GameContext *context){
    char turn_buffer[TURN_BUFFER_LENGTH] = {0};

    printf("\nTurn: ");

    fgets(turn_buffer, TURN_BUFFER_LENGTH, stdin);

    bool valid = is_valid_turn_input(turn_buffer);

    Turn turn = make_turn(context,
                          (Vec2){
                                  .x = ascii_alpha_to_num(turn_buffer[0]),
                                  .y = FIELD_SIZE - ascii_num_to_num(turn_buffer[1]),
                          },
                          (Vec2){
                                  .x = ascii_alpha_to_num(turn_buffer[2]),
                                  .y = FIELD_SIZE - ascii_num_to_num(turn_buffer[3]),
                          });
    turn.valid = valid;
    
    if (!valid){
        return turn;
    }
    
    return turn;
}

static char* get_piece_symbol(Piece piece){
    switch (piece.side) {

        case SIDE_WHITE:
            switch (piece.type) {
                case PIECE_KING:   return "♔";
                case PIECE_QUEEN:  return "♕";
                case PIECE_ROOK:   return "♖";
                case PIECE_KNIGHT: return "♘";
                case PIECE_BISHOP: return "♗";
                case PIECE_PAWN:   return "♙";
                default:           return " ";
            }

        case SIDE_BLACK:
            switch (piece.type) {
                case PIECE_KING:   return "♚";
                case PIECE_QUEEN:  return "♛";
                case PIECE_ROOK:   return "♜";
                case PIECE_KNIGHT: return "♞";
                case PIECE_BISHOP: return "♝";
                case PIECE_PAWN:   return "♟";
                default:           return " ";
            }

        default: return " ";
    }
}

static const PieceType promote_piece_symbols[] = {PIECE_QUEEN, PIECE_ROOK, PIECE_BISHOP, PIECE_KNIGHT};

PieceType get_promotion_option(Piece piece){
    printf("\nPromote pawn to %s ", get_piece_symbol(make_piece(promote_piece_symbols[0], piece.side)));

    clear_stdin();
    
    size_t index = 0;
    
    Key key;
    
    do {
        key = get_key_pressed();
        
        switch(key){

            case KEY_ARROW_RIGHT:{
                index = (index + 1) % 4;
            } break;

            case KEY_ARROW_LEFT:{
                index = (index - 1) % 4;
            } break;

            default: break;
        }

        printf("\rPromote pawn to ");
        printf("%s ", get_piece_symbol(make_piece(promote_piece_symbols[index], piece.side)));
    } while(key != KEY_ENTER);

    switch (index) {
        case 0: return PIECE_QUEEN;
        case 1: return PIECE_KNIGHT;
        case 2: return PIECE_BISHOP;
        case 3: return PIECE_ROOK;
        default: return PIECE_NONE;
    }
}

static String_Builder white_sb = {0};
static String_Builder black_sb = {0};

void init(){
    end();
    
    setlocale(LC_ALL, "en_US.UTF-8");

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    
    sb_appendf(&white_sb, "WHITE");
    sb_appendf(&black_sb, "BLACK");
}

void print_field(GameContext *context){
    String_View line = {0};

    buffer.count = 0;
    size_t line_number = 0;

    sb_appendf(&buffer, "  ┌────┬────┬────┬────┬────┬────┬────┬────┐\n");

    for (int rank = 0; rank < FIELD_SIZE; ++rank) {
        sb_appendf(&buffer, "%d │ ", FIELD_SIZE - rank);

        for (int file = 0; file < FIELD_SIZE; file++) {
            sb_appendf(&buffer, get_piece_symbol(context->field[rank][file]));

            if (file < FIELD_SIZE - 1){
                sb_appendf(&buffer, "  │ ");
            }
        }

        sb_appendf(&buffer, "  │\n");

        if (rank < FIELD_SIZE - 1){
            sb_appendf(&buffer, "  ├────┼────┼────┼────┼────┼────┼────┼────┤");
        }else{
            sb_appendf(&buffer, "  └────┴────┴────┴────┴────┴────┴────┴────┘");
        }


        sb_get_line(&buffer, &line, line_number++);

        sb_appendc(&buffer, '\n');
    }

    sb_appendf(&buffer, "    ");
    for (int file = 0; file < FIELD_SIZE; file++) {
        sb_appendf(&buffer, "%c    ", 'A' + file);
    }

    sb_get_line(&buffer, &line, 0);

    printf("%s\n", buffer.items);
}

void update(GameContext *context){
    print_field(context);

    if (context->mate_side != SIDE_NONE){
        printf("\nMATE!\n");
        return;
    }
    
    if (context->check_side != SIDE_NONE){
        printf("\nCHECK!\n");
    }
}

void end(){
    printf("\x1b[2J\x1b[3J\x1b[H");
}

void update_and_wait_for_input(GameContext *context){
    end();

    update(context);

    getch();
}