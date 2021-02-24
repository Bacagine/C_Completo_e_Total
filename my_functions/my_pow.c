#!/bin/tcc -run

#include <stdio.h>

int my_pow(int a, int b);

int main(void){
	int pow = my_pow(2,2);
	printf("%d\n", pow);
	return 0;
}

int my_pow(int a, int b){
	register int t = 1;
	for(; b; b--){
		t = t*a;
	}
	return t;
}
