#include <stdlib.h>
#include <stdio.h>

int main() {
    int test = ((int)'D' + (int)'Q')%4;

    printf("%d", test);
    return 0;
}
