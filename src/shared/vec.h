#ifndef CHESS_VEC_H
#define CHESS_VEC_H

#include <stdbool.h>

typedef struct {
    int x;
    int y;
} Vec2;

Vec2 vec2_create(int x, int y);
Vec2 vec2_subtract(Vec2 to, Vec2 from);
bool vec2_eq(Vec2 first, Vec2 second);

#ifdef VEC_IMPLEMENTATION

Vec2 vec2_create(int x, int y){
    return (Vec2){
            .x = x,
            .y = y,
    };
}

bool vec2_eq(Vec2 first, Vec2 second){
    return (first.x == second.x)
           && (first.y == second.y);
}

Vec2 vec2_subtract(Vec2 to, Vec2 from){
    return (Vec2){
            .x = to.x - from.x,
            .y = to.y - from.y,
    };
}

#endif // VEC_IMPLEMENTATION

#endif //CHESS_VEC_H