#include <stdio.h>

int main(void) {
    FILE *f = fopen("test0_c2.out", "rb");
    unsigned char nivel;
    int size;
    fread(&size, sizeof(int), 1, f);
    printf("%d ", size);
    for (int i = 0; i < 17; i++) {
        fread(&nivel, sizeof(char), 1, f);
        printf("%d ", nivel);
    }
    return 0;
}
