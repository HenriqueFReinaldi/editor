#include "rw/readwrite.h"
#include "gap/gap.h"
#include "gap/render.h"
#include "util/util.h"

#include <windows.h>
#include <conio.h>

//info
    //geral
    #define VER "29.03.2026.1"
    char* editor_file_path;

    //cmdbar
    #define CMDBARDOWN 3
    char last_cmd_bar_msg[500];
    #define LCBMINFO last_cmd_bar_msg, 500

    //config
    extern int relative_mode;

//gbs

#define EDITORBUFSIZE 100
#define BARBUFSIZE 20
GapBuffer editor;
GapBuffer bar;
GapBuffer* gb;


//configs
int show_gap_buffer = 0;
int down_offset = 0;
int command_bar_mode = 0;


int loadFile(GapBuffer* gb, char* path){
    initGb(gb, EDITORBUFSIZE);
    size_t content_size;
    char* content;

    FILE* f;
    f = fopen(path, "rb");
    if (!f){
        return 1;
    }
    readFile(f, &content_size, &content);
    fclose(f);
    insertString(gb, content, content_size);
    return 0;
}

//
void handleKBInput(char c){

    switch (c){
        case 13:
            insertChar(gb, '\n');
            break;
        case -121:
            insertChar(gb, -61);
            insertChar(gb, -89);
            break;
        case 9:
            insertString(gb, "    ", 4);
            break;
        case 8:
            deleteChar(gb);
            break;
        default:
            insertChar(gb, c);
    }
    
    render(*gb, show_gap_buffer, down_offset);
}

void handleSPInput(char c){
    if (c == 75) moveLeft(gb);
    else if (c == 77) moveRight(gb);
    else if (c == 72) moveUp(gb);
    else if (c == 80) moveDown(gb);

    render(*gb, show_gap_buffer, down_offset);
}


typedef enum {
    CONT,
    BREAK,
    EXIT,
} BarAcReturn;

BarAcReturn handleBarActions(char* command){
    if (strcmp(command, "s") == 0 || strcmp(command, "S") == 0){
        gb = &editor;

        FILE* f = fopen(editor_file_path, "wb");
        if (!f){
            snprintf(LCBMINFO, "Arquivo nao encontrado: %s", editor_file_path);
            gb = &bar;
            return CONT;
        }
        char* text = getText(*gb);
        writeFile(f, text);
        fclose(f);
        free(text);

        snprintf(LCBMINFO, "Salvo em: %s", editor_file_path);

        gb = &bar;
        return CONT;
    }

    if (strcmp(command, "g") == 0 || strcmp(command, "G") == 0){
        show_gap_buffer = !show_gap_buffer;
    }
    
    if (strcmp(command, "v") == 0 || strcmp(command, "V") == 0){
        gb = &editor;
        down_offset = 0;

        system("cls");
        render(*gb, show_gap_buffer, down_offset);
        return BREAK;
    }

    if (strcmp(command, "rel") == 0){
        relative_mode = !relative_mode;
        if (relative_mode) snprintf(LCBMINFO, "Linhas em modo relativo");
        else snprintf(LCBMINFO, "Linhas em modo absoluto");
        return CONT;
    }

    if (strcmp(command, "ajuda") == 0){
        snprintf(LCBMINFO, 
            "Lista de comandos:\n"
            "'s'      : salvar\n"
            "'v'      : voltar\n"
            "'sair'   : fecha o programa\n"
            "'rel'    : alterna entre linhas relativas e absolutas\n\n"

            "Comandos com argumentos:\n"
            "'run'    : abre o terminal e roda um comando\n"
            "'change' : troca o arquivo\n"
            "'cursor' : troca o cursor\n"
            "'color'  : troca a cor de destaque"
        );
        return CONT;
    }

    if (strcmp(command, "sair") == 0){
        handleBarActions("s");
        return EXIT;
    }

    if (startsWith(command, "run ") == 1){
        char* rest = command+3;
        size_t len_command = strlen(rest);
        char buffer[14+len_command];

        snprintf(buffer, 14+len_command, "start cmd /k %s", rest);
        system(buffer);
        return CONT;
    }
    
    if (startsWith(command, "change ") == 1){
        char* temp = command+7;
        size_t len_path = strlen(temp)+1;


        char* new_path = malloc(len_path*sizeof(char));
        snprintf(new_path, len_path, "%s", command+7);


        if (loadFile(&editor, new_path) == 0){
            free(editor_file_path);
            editor_file_path = new_path;
            snprintf(LCBMINFO, "Agora editando: %s", editor_file_path);
            return CONT;
        }

        snprintf(LCBMINFO, "Nao encontrado: %s", new_path);
        return CONT;
    }

    if (startsWith(command, "color ") == 1){
        char* temp = command+6;
        size_t len_cor = strlen(temp)+1;
        char* new_cor = malloc(len_cor*sizeof(char));
        snprintf(new_cor, len_cor, "%s", command+6);

        snprintf(LCBMINFO, "Nova cor: %s", new_cor);
        if (strcmp(new_cor, "red") == 0) strncpy(GBACCENT, GBRED, 6);
        else if (strcmp(new_cor, "blue") == 0) strncpy(GBACCENT, GBBLU, 6);
        else if (strcmp(new_cor, "yellow") == 0) strncpy(GBACCENT, GBYEL, 6);
        else if (strcmp(new_cor, "green") == 0) strncpy(GBACCENT, GBGRN, 6);
        else if (strcmp(new_cor, "purple") == 0) strncpy(GBACCENT, GBPUR, 6);
        else if (strcmp(new_cor, "white") == 0) strncpy(GBACCENT, GBRESET, 6);
        else{
        snprintf(LCBMINFO, "Cor '%s' não existe.", new_cor);
        }

        return CONT;
    }

    if (startsWith(command, "cursor ") == 1){
        char* temp = command+7;
        size_t len = strlen(temp)+1;
        char* new_cursor = malloc(len*sizeof(char));
        snprintf(new_cursor, len, "%s", command+7);

        free(cursor);
        cursor = new_cursor;

        return CONT;
    }

    snprintf(LCBMINFO, "Comando desconhecido.");
    return CONT;
}





int main(int argc, char** argv){
    SetConsoleOutputCP(CP_UTF8);
    printf("\e[1;1H\e[2J");
    printf("\033[?25l");

    initGb(&editor, EDITORBUFSIZE);
    initGb(&bar, BARBUFSIZE);


    //
    relative_mode = 0;

    cursor = malloc(2*sizeof(char));
    strncpy(cursor, "<", 2);

    

    GBACCENT = malloc(6*sizeof(char));
    strncpy(GBACCENT, GBBLU, 6);
    GBACCENT_SIZE = 5;

    //

    gb = &editor;

    if (argc == 2){
        char* temp = argv[1];
        size_t len_path = strlen(temp)+1;

        char* new_path = malloc(len_path*sizeof(char));
        snprintf(new_path, len_path, "%s", argv[1]);
        editor_file_path = new_path;

        loadFile(gb, editor_file_path);
    }

    //moveStart(gb);
    

    render(*gb, show_gap_buffer, down_offset);

    while (1) {
        if (_kbhit()){
            char c = _getch();

            if (command_bar_mode && c == 13){
                char* bar_text = getText(*gb);
                BarAcReturn r = handleBarActions(bar_text);
                free(bar_text);

                if (r == BREAK){
                    system("cls");
                    gb = &editor;
                    down_offset = 0;
                    command_bar_mode = 0;
                    render(*gb, show_gap_buffer, down_offset);
                }
                if (r == EXIT){
                    system("cls");
                    printf("\033[?25h");
                    break;
                }
                
                initGb(&bar, BARBUFSIZE);
                system("cls");
                render(*gb, show_gap_buffer, down_offset);
            }

            else if (c == 27) { //entrar bar mode
                system("cls");
                gb = &bar;
                down_offset = CMDBARDOWN;
                command_bar_mode = 1;
                
                render(*gb, show_gap_buffer, down_offset);
            }   

            else if (c == -32){
                handleSPInput(_getch());
            }
            else handleKBInput(c);

            if (command_bar_mode){
                printf("\033[1;1H");
                printf("versão " VER ". Execute 'ajuda' para saber mais.");

                for (int i = 0; i < CMDBARDOWN+1; i++)printf("\n");

                printf("%s", last_cmd_bar_msg);
            }
        }
        
    }
    return 0;
}