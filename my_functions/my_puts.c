#!/usr/bin/tcc -run
#include <stdio.h>

// Indexa s como uma matriz
void my_puts(char *s);
void my_puts2(char *s);

int main(void){
	my_puts("Ola Mundo!!!\n");
	my_puts2("Ola Mundo 2!!!\n");
	return 0;
}

void my_puts(char *s){
	register int t;
	for(t = 0; s[t]; ++t){
		putchar(s[t]);
	}
}

/* Os programadores consideram 
 * essa versão mais fácil de 
 * entender e ler*/
void my_puts2(char *s){
	while(*s){
		putchar(*s++);
	}
}

