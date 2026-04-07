#include "gap.h"

int selection_start = -1;

//init
void initGb(GapBuffer* gb, size_t gap_size){
    if (gap_size < 1) gap_size = 1;

    gb->buffer = calloc(gap_size, gap_size*sizeof(char));
    gb->buffer_size = gap_size;
    gb->gap_size = gap_size;
    gb->gapl = 0;
    gb->gapr = gap_size-1;
}


//getting content
char* getText(GapBuffer gb){
    char* result = malloc(gb.buffer_size*sizeof(char)+1);

    size_t result_index = 0;
    for(size_t i = 0; i < gb.gapl; i++){
        if (gb.buffer[i] != 0) result[result_index++] = gb.buffer[i];
    }
    for(size_t i = gb.gapr+1; i < gb.buffer_size; i++){
        if (gb.buffer[i] != 0) result[result_index++] = gb.buffer[i];
    }
    result[result_index] = '\0';

    return result;
}

//grow
void grow(GapBuffer* gb){
    char* temp = realloc(gb->buffer, gb->buffer_size+gb->gap_size);
    gb->buffer = temp;
    gb->buffer_size += gb->gap_size;

    for (size_t i = gb->buffer_size-1; i > gb->buffer_size-gb->gap_size; i--){
        gb->buffer[i] = 0;
    }

    if (gb->gapr < gb->buffer_size-gb->gap_size-1){
        size_t afteri = gb->gapl+1;
        size_t l = gb->buffer_size-gb->gap_size-1-gb->gapr;

        memmove(gb->buffer+(gb->buffer_size-l), gb->buffer+afteri, l);  
        memset(gb->buffer+afteri, '\0', gb->gap_size);
    }

    gb->gapr += gb->gap_size;
}

//insert, delete
void insertChar(GapBuffer* gb, char c){
    if (gb->gapl == gb->gapr){
        grow(gb);
    }

    gb->buffer[gb->gapl] = c;
    gb->gapl++;
}

void insertString(GapBuffer* gb, char* text, size_t size){
    for (size_t i = 0; i < size; i++){
        insertChar(gb, text[i]);
    }
}

void deleteChar(GapBuffer* gb){
    if (gb->gapl == 0){
        return;
    }

    gb->buffer[gb->gapl-1] = 0;
    gb->gapl--;
}


//movingfs
void moveLeft(GapBuffer* gb){
    if (gb->gapl == 0){
        return;
    }

    gb->gapl--;
    char temp = gb->buffer[gb->gapl];
    gb->buffer[gb->gapl] = gb->buffer[gb->gapr];
    gb->buffer[gb->gapr] = temp;
    gb->gapr--;
}

void moveRight(GapBuffer* gb){
    if (gb->gapr == gb->buffer_size-1){
        return;
    }

    gb->gapr++;
    char temp = gb->buffer[gb->gapr];
    gb->buffer[gb->gapr] = gb->buffer[gb->gapl];
    gb->buffer[gb->gapl] = temp;
    gb->gapl++;
}


void moveToCol(GapBuffer* gb, int col){
    while((gb->gapl > 0) && (gb->buffer[gb->gapl-1] != '\n')){
        moveLeft(gb);
    }

    while((gb->buffer[gb->gapr+1] != '\n') && col-- > 0){
        moveRight(gb);
    }
}

void moveUp(GapBuffer* gb){
    int col = 0;

    while((gb->gapl > 0) && (gb->buffer[gb->gapl-1] != '\n')){
        moveLeft(gb);
        col++;
    }


    if (gb->gapl > 0)moveLeft(gb);

    moveToCol(gb, col);
}

void moveDown(GapBuffer* gb){
    int col = 0;
    int index = gb->gapl;
    while((index  > 0) && (gb->buffer[index-1] != '\n')){
        index--;
        col++;
    }

    while((gb->gapr < gb->buffer_size-1) && (gb->buffer[gb->gapr+1] != '\n')){
        moveRight(gb);
    }
    if (gb->gapr < gb->buffer_size-1)moveRight(gb);

    moveToCol(gb, col);
}

void moveStart(GapBuffer* gb){
    while(gb->gapl != 0) moveLeft(gb);
}


CharType get_char_type(char c){
    if (isspace(c)) return SPACE;
    if (isalnum(c) || c == '_' || c == '\'' || c == '"') return ALNUM;
    return OTHER;
}


void moveUpAbsolute(GapBuffer* gb){
    while((gb->gapl > 0) && (gb->buffer[gb->gapl-1] != '\n')) moveLeft(gb);
    if (gb->gapl > 0)moveLeft(gb);
        
}

void moveDownAbsolute(GapBuffer* gb){
    while((gb->gapr < gb->buffer_size-1) && (gb->buffer[gb->gapr+1] != '\n')) moveRight(gb);
    if (gb->gapr < gb->buffer_size-1)moveRight(gb);

    while((gb->gapr < gb->buffer_size-1) && (gb->buffer[gb->gapr+1] != '\n')) moveRight(gb);
}

void moveLeftWord(GapBuffer* gb){
    if (gb->gapl == 0) return;

    CharType past_char_type, current_char_type;
    current_char_type = get_char_type(gb->buffer[gb->gapl-1]);
    past_char_type = current_char_type;


    while(past_char_type == current_char_type && gb->gapl > 0){
        moveLeft(gb);
        past_char_type = current_char_type;
        current_char_type = get_char_type(gb->buffer[gb->gapl-1]);
    } 
}

void moveRightWord(GapBuffer* gb){
    if (gb->gapr >= gb->buffer_size-1) return;
    CharType past_char_type, current_char_type;

    current_char_type = get_char_type(gb->buffer[gb->gapr+1]);
    past_char_type = current_char_type;

    while(past_char_type == current_char_type && gb->gapr < gb->buffer_size-1){
        moveRight(gb);
        past_char_type = current_char_type;
        current_char_type = get_char_type(gb->buffer[gb->gapr+1]);
    } 
}