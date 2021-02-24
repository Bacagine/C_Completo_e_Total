#include <stdio.h>
#include <math.h>

#undef abs

#define abs(i) (i)<0 ? -(1) : (i)

int main(void){
	int num_neg = -1;
	int num_posi;
	
	num_posi = abs(num_neg);
	printf("Valor de num agora é: %d\n", num_neg);
	puts(num_posi);

	return 0;
}

