#include "render.h"

//TELA TERMINAL==========================================
#define LINES 23 
#define SCREEN_SIZE 2001 //incluir terminador nulo
char screen[SCREEN_SIZE] = {0};

char new_screen[SCREEN_SIZE] = {0};
int new_screen_ignore_count[LINES];
size_t new_screen_index = 0;
size_t nsig_index = 0;

void terminalRenderScreen(int down_offset){
    printf("\033[%d;1H", down_offset);

    size_t sl = 0;
    int screen_lines_size[LINES] = {0};
    size_t current_screen_ls = 0;

    size_t nsl = 0;
    int new_screen_lines_size[LINES] = {0};
    size_t new_screen_ls = 0;

    for (int i = 0 ; i < SCREEN_SIZE; i++){
        if (screen[i] == '\n' || screen[i] == '\0'){
            if (sl < LINES) screen_lines_size[sl++] = current_screen_ls;
            current_screen_ls = 0;
        }
        else current_screen_ls++;


        if (new_screen[i] == '\n' || new_screen[i] == '\0'){
            if (nsl < LINES) {
                int ignore_count = new_screen_ignore_count[nsl];
                new_screen_lines_size[nsl++] = new_screen_ls-ignore_count;
            }
            new_screen_ls = 0;
        } 
        else new_screen_ls++;

        screen[i] = new_screen[i];
    }


    // printf("\n\n%lld %lld\n", sl, nsl);
    // for (int i = 0; i < LINES; i++){
    //     printf("[%d]", new_screen_ignore_count[i]);
    // }
    // printf("\n");


    // printf("\nlinhassize: ");
    // for (size_t i = 0 ; i < nsl; i++){
    //     printf("[%d]", new_screen_lines_size[i]);
    // }
    // printf("\n");


    size_t diff_size = sl;
    int line_size_diff[LINES] = {0};


    for (size_t i = 0 ; i < diff_size; i++){
        line_size_diff[i] = new_screen_lines_size[i]-screen_lines_size[i];
    }
    

    char tela_buffer[SCREEN_SIZE] = {0};
    size_t buf_i = 0;
    size_t lines_seen = 0;

    for (int i = 0; i < SCREEN_SIZE; i++){
        if (new_screen[i] == '\n' || new_screen[i] == '\0'){
            for (int j = 0; j > line_size_diff[lines_seen]; j--) {
                if (buf_i == SCREEN_SIZE-1) goto fim_funcao;
                tela_buffer[buf_i++] = ' ';
            }
            lines_seen++;
            if (new_screen[i] == '\0' || buf_i == SCREEN_SIZE-1) break;
            tela_buffer[buf_i++] = '\n';
        }
        else{
            if (buf_i == SCREEN_SIZE-1) break;
            tela_buffer[buf_i++] = new_screen[i];
        }
    }

    for (size_t i = lines_seen; i < diff_size; i++){

        if (buf_i == SCREEN_SIZE-1) goto fim_funcao;
        tela_buffer[buf_i++] = '\n';

        for (int j = 0; j > line_size_diff[i]; j--){
            if (buf_i == SCREEN_SIZE-1) goto fim_funcao;
            tela_buffer[buf_i++] = ' ';
        }
    }

    // printf("(visto:%lld total:%lld falta:%lld)\n", lines_seen, diff_size, diff_size-lines_seen);

    fim_funcao:
    // printf("\n[%s]\n", tela_buffer);


    printf("%s", tela_buffer);
}
//======================================================


#define LINEQTD 10
#define LINEPADDING 5
char* cursor;
char* GBACCENT;
size_t GBACCENT_SIZE;

int relative_mode = 0;
int current_line;
int above_count;
int below_count;



void putCharNS(char c){
    if (new_screen_index >= SCREEN_SIZE-1) return;

    new_screen[new_screen_index++] = c;
}

void putStringNS(char* str){
    if (new_screen_index >= SCREEN_SIZE-1) return;

    int length = strlen(str);
    for (int i = 0; i < length; i++){
        new_screen[new_screen_index++] = str[i];
    }
}



void putLineCount(){
    int grayed = 1;
    int current = 0;

    if (relative_mode){
        if (above_count > 0){
        current = above_count;    
        above_count--;
        }
        else{
            current = below_count++;
        }    
        if (current == 0){
            putStringNS(GBACCENT);
            grayed = 0;
            current = current_line;
        }     
    }    
    else{
        if (above_count > 0){
        current = current_line-above_count;    
        above_count--;
        }
        else{
            current = current_line+below_count++;
        }    
        if (current == current_line){
            putStringNS(GBACCENT);
            grayed = 0;
        }     
    }    
                
    if (grayed) putStringNS(GBLGRAY);

    if (!relative_mode) current++;
    char num[20] = {0};
    int length = snprintf(num, 20, "%d", current);
    putStringNS(num);


    for (int i = length; i < LINEPADDING; i++)putCharNS(' ');

    putStringNS(GBRESET " ");
}    


int handleChar(char c){
    if (c == '\r');
    else if (c == '\n'){
        putCharNS(c);
        putLineCount();
    }
    else if (isprint(c) || c == '\t' || c < 0) putCharNS(c);
    else {
        char num[20] = {0};
        snprintf(num, 20, "%d", c);
        putCharNS('[');
        putStringNS(num);
        putCharNS(']');
    }
    return 0;
}

void render(GapBuffer gb, int s, int down_offset){
    above_count = 0;
    below_count = 0;
    current_line = 0;

    new_screen_index = 0;
    nsig_index = 0;
    for (int i = 0; i < SCREEN_SIZE; i++){
        new_screen[i] = 0;
        if (i < LINES) new_screen_ignore_count[i] = 0; 
    }

    
    size_t start_index = gb.gapl;
    size_t max_index = gb.gapr+1;
    size_t line_count = 0;

    for(size_t i = 0; i < start_index; i++) if(gb.buffer[i] == '\n') current_line++;

    for(; (line_count <= LINEQTD); start_index--){
        if(gb.buffer[start_index] == '\n'){
            line_count++;
            above_count++;
        }
        if (start_index == 0) break;
    }


    if (line_count > LINEQTD) start_index+=2;

    for(; (max_index < gb.buffer_size && line_count <= LINEQTD*2+1); max_index++){   
        if(gb.buffer[max_index] == '\n')line_count++;
    }

    if (above_count > LINEQTD) above_count = LINEQTD;


    //remover cursor da contagem
    new_screen_ignore_count[above_count] = GBACCENT_SIZE+5;

    // printf("\n\n");
    // printf("%d %d %d", above_count, current_line, below_count);

    //render 
    putLineCount();

    for(size_t i = start_index; i < gb.gapl; i++){
        handleChar(gb.buffer[i]);
    }

    


    putStringNS(GBACCENT);
    putStringNS(cursor);
    putStringNS(GBRESET);

    //DEBUG
    if (s){
        putStringNS(GBACCENT);
        for(size_t i = gb.gapl; i < gb.gapr+1; i++) putStringNS("_");

        char debug_info[230] = {0};
        snprintf(debug_info, 230, "(%lld %lld %lld)" GBRESET,gb.gapl, gb.gapr, gb.buffer_size);

        putStringNS(debug_info);
        putStringNS(GBRESET);

    }


    for(size_t i = gb.gapr+1; i < max_index; i++){
        handleChar(gb.buffer[i]);
    }

    terminalRenderScreen(down_offset);
}