#include "util.h"

int startsWith(char* a, char* b){
    size_t lena = strlen(a);
    size_t lenb = strlen(b);

    if (lenb > lena) return 0;
    for (size_t i = 0 ; i < lenb; i++){
        if (a[i] != b[i]) return 0;
    }
    return 1;
}