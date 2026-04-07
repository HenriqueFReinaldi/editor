#ifndef GAPBUFF
#define GAPBUFF

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#ifdef COLORLESS
    #define GBRED ""
    #define GBBLU ""
    #define GBGRN ""
    #define GBYEL ""
    #define GBPUR ""

    #define GBLGRAY ""
    #define GBRESET ""
    #define GBACCENT ""
#else
    #define GBRED "\x1b[31m"
    #define GBBLU "\x1b[34m"
    #define GBGRN "\x1b[32m"
    #define GBYEL "\x1b[33m"
    #define GBPUR "\x1b[35m"

    #define GBBG "\x1b[44m"

    #define GBLGRAY "\x1b[90m"
    #define GBRESET "\x1b[0m"
    extern char* GBACCENT;
    extern size_t GBACCENT_SIZE;
#endif
    #define GBCOLOR_SIZE 5

typedef enum{
    ALNUM,
    SPACE,
    OTHER,
} CharType;

typedef struct{
    char* buffer;
    size_t buffer_size;
    size_t gap_size; //const

    size_t gapl; //index gapleft
    size_t gapr; //gapright
} GapBuffer;


extern int selection_start;

void initGb(GapBuffer* gb, size_t gap_size);

//getting content
char* getText(GapBuffer gb);

//grow
void grow(GapBuffer* gb);

//insert, delete
void insertChar(GapBuffer* gb, char c);
void insertString(GapBuffer* gb, char* text, size_t size);
void deleteChar(GapBuffer* gb);

//movingfs
void moveLeft(GapBuffer* gb);
void moveRight(GapBuffer* gb);

void moveToCol(GapBuffer* gb, int col);
void moveUp(GapBuffer* gb);
void moveDown(GapBuffer* gb);

void moveStart(GapBuffer* gb);

void moveLeftWord(GapBuffer* gb);
void moveRightWord(GapBuffer* gb);

void moveUpAbsolute(GapBuffer* gb);
void moveDownAbsolute(GapBuffer* gb);


#endif