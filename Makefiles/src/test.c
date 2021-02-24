#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include "test.h"

void test2(void), test3(void);

int count = 0;

int main(void){
    
    system("clear");
    
    printf("count = %d\n", count);
    test2();
    printf("count = %d\n", count);
    test3();
    printf("count = %d\n", count);
    
    getchar();
    
    return 0;
}
