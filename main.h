#ifndef MAIN_H
#define MAIN_H

#include "gba.h"
//contains function prototypes and structs
struct strawberry {
    u16 color;
    int height;
    int width; 
    int row;
    int col;
};

struct basket {
    int size;
    int row;
    int col;
};

struct state{
    int size;
    struct basket mainBasket;
};

