#include <stdio.h>
extern int Sum(void);

int main(void){
    printf("Dynamic linking\n");
    Sum(); 
    return 0;
}