#include <stdio.h>
#include <errno.h>

int main(void){
	FILE * fp;
	fp = fopen("arq.txt", "r");
	if(fp == NULL){
		printf("Codigo do erro: %d\n", errno);
	}
	else{
		printf("O arquivo ja existe.\n");
	}

	return 0;
}

