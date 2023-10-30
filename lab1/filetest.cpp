#include <stdio.h>

int main(){
    FILE *file = fopen(".\\a.txt","wb");
    fprintf(file, "Hello\n");
}
