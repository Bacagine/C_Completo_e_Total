/* matriz.c: alocação dinamica de matrizes em C
 * 
 * Criado por Gustavo Bacagine <gustavo.bacagine@protonmail.com>
 * 
 * Data: 13/10/2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void cria_matriz(int **p, int lin, int col);
void mostra_matriz(int **p, int lin, int col);

int main(void){
	int length = 3;
	int **p = (int **) malloc(length * sizeof(int *));
	for(int i = 0; i < length; i++){
		p[i] = (int *) malloc(length * sizeof(int));
	}

	if(p == NULL){
		fprintf(stderr, "Erro: Memoria insuficiente!\n");
		exit(1);
	}

	cria_matriz(p, 3, 3);
	mostra_matriz(p, 3, 3);
	
	free(p);

	return 0;
}

void cria_matriz(int **p, int lin, int col){
	srand(time(0));
	for(int i = 0; i < lin; i++){
		for(int j = 0; j < col; j++){
			p[i][j] = rand() % 10;
		}
	}
}

void mostra_matriz(int **p, int lin, int col){
	for(int i = 0; i < lin; i++){
		for(int j = 0; j < col; j++){
			printf("%d\t", p[i][j]);
		}
		putchar('\n');
	}
}

