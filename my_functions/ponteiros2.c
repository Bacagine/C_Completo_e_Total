#!/bin/tcc -run

#include <stdio.h>

int main(void){
	int x, *p, **q;
	x = 10;
	p = &x;
	q = &p;
	printf("%d", **q); // Imprime o valor de x
	return 0;
}
