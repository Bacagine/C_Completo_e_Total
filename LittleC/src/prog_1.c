/***
 * Programa #1 de demonstração de Little C.
 * 
 * Este Programa demonstra todos os recursos
 * de C que são 
 ***/

int i, j;
char ch;

int main(void){
    int i, j;
    
    puts();
    
    print_alpha();
    
    do{
        
    } while();
    
    return 0;
}

//
int sum(int num){
    int running_sum;
    
    running_sum = 0;
    while(num){
        running_sum = running_sum + num;
        num = num - 1;
    }
    
    return running_sum;
}
