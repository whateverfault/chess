#ifndef CHESS_CONSOLE_H
#define CHESS_CONSOLE_H

typedef enum {
    KEY_UNKNOWN = 0,
    KEY_ENTER = 13,
    KEY_SPACE = 32,
    KEY_ARROW_UP = 72,
    KEY_ARROW_DOWN = 80,
    KEY_ARROW_RIGHT = 77,
    KEY_ARROW_LEFT = 75,
} Key;

Key get_key_pressed();

#ifdef CONSOLE_IMPLEMENTATION

#include <conio.h>

Key get_key_pressed(){
    int ch = _getch();

    if (ch == 0 || ch == 224){
        ch = _getch();
    }

    return (Key)ch;
}

#endif // CONSOLE_IMPLEMENTATION

#endif //CHESS_CONSOLE_H
