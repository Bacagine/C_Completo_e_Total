#!/usr/bin/tcc -run
#include <stdio.h>
#include <stdlib.h>
#define L 3
#define C 10

int main(void){
    int M[L][C] = { { 482, 502, 487, 352, 323, 1138, 1119, 1074, 1146, 1222 },
                    { 17050, 19046, 20093, 19118, 20463, 25305, 26756, 28879, 32525, 33725 },
                    { 87, 101, 55, 75, 146, 88, 79, 132, 88, 98 } };

//    int x,y,somaL,somaC;
    int somaL = 0, somaC = 0;
    int soma;
//    int l = 0;
/*    // entrando com os dados da matriz 7x6 via parametro
    printf("Digite ELEMENTO da:\n");
    for(x=0;x<L;x++){
        for(y=0;y<C;y++){
            printf("linha %d, coluna %d da matriz: ",x,y);
            scanf("%d",&M[x][y]);
        }
    }*/
    //acumulando a soma da linha 5 e da coluna 3

    for (int i = 0;i < L; i++){
        for(int j = 0;j < C; j++){
            somaC += M[i][j];
        }
        printf ("%d \n", somaC);
        somaC = 0;
    }
	
	/*for(int i = 0, j = 0; i<L, j<C; i++, j++){
		somaC += M[L][j];
		somaL += M[i][C];
	}
	soma = somaC + somaL;
	printf("A soma total é: %d", soma);*/

    printf("\n\n");

}

