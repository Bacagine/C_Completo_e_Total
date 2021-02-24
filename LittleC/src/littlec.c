/***********************************************************
 * Github: https://github.com/Bacagine/C_Completo_e_Total  *
 *                                                         *
 * littlec.c: Little C é um interpretador da linguagem C   *
 * criado por Herbert Schildt                              *
 *                                                         *
 * Este código fonte pode ser encontrado no livro:         *
 * "C Completo e Total" Parte 5                            *
 *                                                         *
 * Este código foi modificado por:                         *
 * Gustavo Bacagine <gustavo.bacagine@protonmail.com>      *
 *                                                         *
 * Data: 07/02/2020                                        *
 ***********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <setjmp.h>

#define NUM_FUNC        100
#define NUM_GLOBAL_VARS 100
#define NUM_LOCAL_VARS  200
#define NUM_BLOCK       100
#define ID_LEN          31
#define FUNC_CALLS      31
#define NUM_PARAMS      31
#define PROG_SIZE       10000
#define LOOP_NEST       31

enum tok_types{ DELTIMER, IDENTIFIER, NUMBER, KEYWORD, TEMP,
                STRING, BLOCK };

// Adicione outros tokens C aqui
enum tokens{ ARG, CHAR, INT, IF, ELSE, FOR, DO, WHILE,
             SWITCH, RETURN, EOL, FINISHED, END };

// Adiciona outros operadores duplos (tais como ->) aqui
enum double_ops{ LT = 1, LE, GT, GE, EQ, NE };

/* Estas são as constantes usadas para chamar sntx_err() quando
 * ocorre um erro de sintaxe. Adicione mais, se desejar.
 * NOTA: SYNTAX é uma mensagem genérica de erro usada quando
 * nenhuma outra parece apropiada. */
enum error_msg{ SYNTAX, UNBAL_PARENS, NO_EXP, EQUALS_EXPECTED,
                NOT_VAR, PARAM_ERR, SEMI_EXPECTED, UNBAL_BRACES,
                FUNC_UNDEF, TYPE_EXPECTED, NEST_FUNC, RET_NOCALL,
                PAREN_EXPECTED, WHILE_EXPECTED, QUOTE_EXPECTED,
                NOT_TEMP, TOO_MANY_LVARS };

char *prog;   // Posição corrente do código fonte
char *p_buf; /* Aponta para o início da área de 
              * carga do programa */
jmp_buf e_buf; // Mantém ambiente para longjmp()

/* Uma matriz destas estruturas manterá a informação
 * assosiada com as variáveis globais */

struct var_type{
    char var_name[ID_LEN];
    int var_type;
    int value;
} global_vars[NUM_GLOBAL_VARS];

struct var_type local_var_stack[NUM_LOCAL_VARS];

struct func_type{
    char func_name[ID_LEN];
    char *loc; /* Posição do ponto de entrada da função no
                  arquivo */
} func_table[NUM_FUNC];

int call_stack[NUM_FUNC];

struct commands{ // Tabela de busca de palavras-chave
    char command[20];
    char tok;
} table[] = {              // Comandos devem ser escritos
    { "if", IF         }, // em minusculo nesta tabela
    { "else", ELSE     },
    { "for", FOR       },
    { "do", DO         },
    { "while", WHILE   },
    { "char", CHAR     },
    { "int", INT       },
    { "return", RETURN },
    { "end", END       },
    { "", END          } // Marca fim da tabela
}; 

char token[80];
char token_type, tok;

int functos; /* Índice para o topo da pilha de chamadas de
              * função */
int func_index;  // Índice na tabela de funções
int gvar_index; // Índice da tabela global de variáveis
int lvartos;   // Índice para a pilha de variáveis locais

int ret_value; // Valor de retorno de função

void print(void), prescan(void);
void decl_global(void), call(void), putback(void);
void decl_local(void), local_push(struct var_type i);
void eval_exp(int *value), sntx_err(int error);
void exec_if(void), find_eob(void), exec_for(void);
void get_params(void), get_args(void);
void exec_while(void), func_push(int i), exec_do(void);
void assign_var(char *var_name, int value), find_var(char *s);
int load_program(char *p, char *fname), fin_var(char *s);
void interp_block(void), func_ret(void);
int func_pop(void), is_var(char *s), get_token(void);

char *find_func(char *name);

int main(int argc, char *argv[]){
    if(argc != 2){
        printf("Uso: littlec <nome_de_arquivo>\n");
        exit(1);
    }
    
    // Aloca memória para o programa
    if((p_buf =  (char *) malloc(PROG_SIZE)) == NULL){
        printf("Falha de alocação");
        exit(1);
    }
    
    // Carrega o programa a executar
    if(!load_program(p_buf, argv[1])) exit(1);
    if(setjmp(e_buf)) exit(1); // Inicializa buffer de long jmp
    
    // Define ponteiro de programa para o início do buffer
    prog  = p_buf;
    prescan(); /* Procura a posição de todas as funções
                * e variáveis globais no programa */
    gvar_index = 0; // Inicializa índice de variáveis globais
    lvartos = 0;   /* Inicializa índice da pilha de variáveis 
                    * locais */
    functos = 0; // Inicializao o índice da pilha de CALL
    
    // prepara chamada de main()
    prog = find_func("main"); /* Acha o ponto de início
                                 do programa */
    prog--; // Volta para inicial ( */
    strcpy(token, "main");
    call();
                                     
    return 0;
}

/* Interpreta um único comando ou bloco de código. Quando
 * interp_block() retorna da sua chamada inicial, a chave
 * final (ou um return) foi encontrada em main(). */
void interp_block(void){
    int value;
    char block = 0;
    
    do{
        token_type = get_token();
        
        /* Se interpretado um único comando,
         * retorne ao achar o primeiro ponto-e-vírgula. */
        
        // Veja que tipo de token está pronto
        if(token_type == IDENTIFIER){
            // Não é uma palavra-chave, logo processa expressão.
            putback(); /* Devolve token para a entrada para 
                        * posterior processamento por eval_exp() */
            eval_exp(&value); // Processa a expressão
            if(*token != ';') sntx_err(SEMI_EXPECTED);
        }
        else if(token_type == BLOCK){ // Se delimitador de bloco
            if(*token == '{'){ // é um bloco
                block = 1; // Interpretando bloco, não comando
            }
            else return; // é um }, portanto devolve
        }
        else // é uma palavra-chave
            switch(tok){
                case CHAR:
                case INT: // Declara variáveis locais
                    putback();
                    decl_local();
                    break;
                case RETURN: // Retorna da chamada de função
                    func_ret();
                    return;
                case IF: // Processa um comando if
                    exec_if();
                    break;
                case ELSE: // Processa um chamado else
                    find_eob(); /* Acha o fim do bloco de else
                                 * e continua execução */
                    break;
                case WHILE: // Processa um laço while
                    exec_while();
                    break;
                case DO: // Processa um laço do-while
                    exec_do();
                    break;
                case FOR: // Processa um laço for 
                    exec_for();
                    break;
                case END:
                    exit(0);
            }
    } while(tok != FINISHED && block);
}

// Carrega um programa
int load_program(char* p, char* fname){
    FILE *fp;
    int i = 0;
    
    if((fp = fopen(fname, "rb")) == NULL) return 0;
    
    i = 0;
    do{
        *p = getc(fp);
        p++, i++;
    } while(!feof(fp) && i < PROG_SIZE);
    if(*(p - 2) == 0x1a) *(p - 2) = '\0'; // Encerra o programa com nulo
    else *(p - 2) = '\0';
    fclose(fp);
    return 1;
}

/* Acha a posição de todas as funções no programa
 * e armazena todas as variáveis globais */
void prescan(void){
    char *p;
    char temp[32];
    int brace = 0; /* Quando 0, esta variável indica que a
                    * posição corrente no código fonte está fora
                    * de qualquer função */
    
    p = prog;
    func_index = 0;
    do{
        while(brace){ /* Deixa de lado o código dentro das
                       * funções */
            get_token();
            if(*token == '{') brace++;
            if(*token == '}') brace--;
        }
        
        get_token();
        
        if(tok == CHAR || tok == INT){ // É uma variável global
            putback();
            decl_global();
        }
        else if(token_type == IDENTIFIER){
            strcpy(temp, token);
            get_token();
            if(*token == '('){ // Tem de ser uma função
                func_table[func_index].loc = prog;
                strcpy(func_table[func_index].func_name, temp);
                func_index++;
                while(*prog != ')') prog++;
                prog++;
                // Agora prog aponta para o abre-chaves da função
            }
            else putback();
        }
        else if(*token == '{') brace++;
    } while(tok != FINISHED);
    prog = p;
}

/* Devolve o ponto de entrada da função especificada. Devolve
 * NULL se não encontrou */
char *find_func(char *name){
    register int i;
    
    for(i = 0, i < func_index, i++){
        if(!strcmp(name, func_table[i].func_name)){
            return func_table[i].loc;
        }
    }
    
    return NULL;
}

// Declara uma variável global
void decl_global(void){
    get_token(); // Obtém o tipo
    
    global_vars[gvar_index].var_type = tok;
    global_vars[gvar_index].value = 0; // Inicializa com 0
    
    do{ // Processa lista separada por vírgulas
        get_token(); // Obtém o nome da variável
        strcpy(global_vars[gvar_index].var_name, token);
        get_token();
        gvar_index++;
    } while(*token == ',');
    if(*token != ';') sntx_err(SEMI_EXPECTED);
}

// Declara uma variável local
void decl_local(void){
    struct var_type i;
    
    get_token(); // Obtém o tipo
    
    i.var_type = tok;
    i.value = 0; // Inicializa com 0
    
    do{ // Processa lista separada por vírgulas
        get_token(); // Obtém o nome da variável
        strcpy(i.var_name, token);
        local_push(i);
        get_token();
        gvar_index++;
    } while(*token == ',');
    if(*token != ';') sntx_err(SEMI_EXPECTED);
}

//
void call(void){
    char *loc, *temp;
    int lvartemp;
    
    loc = find_func(token); /* Encontra ponto de entrada da
                             * função */
    if(loc == NULL){
        sntx_err(FUNC_UNDEF); // Função não definida
    }
    else{
        lvartemp = lvartos; /* Guarda índice da pilha de var
                             * locais */
        
        get_args();   // Obtém argumentos da função
        temp = prog; // Salva endereço de retorno
        func_push(lvartemp); /* Salva índice da pilha de var
                              * locais */
        prog = loc;    // Redefine prog para o início da função
        get_params(); /* Carrega os parâmetros da função com
                       * os valores dos argumentos */
        interp_block();         // Interpreta a função
        prog = temp;           // Redefine o ponteiro de programa
        lvartos = func_pop(); // Redefine a pilha de var locais
    }
}

/* Empilha os argumentos de uma função na pilha de variáveis
 * locais */
void get_args(void){
    int value, count, temp[NUM_PARAMS];
    struct var_type i;
    
    count = 0;
    get_token();
    if(*token != '(') sntx_err(PAREN_EXPECTED);
    
    // Processa uma lista de valores separados por vírgulas
    do{
        eval_exp(&value);
        temp[count] = value; // Salva temporariamente
        get_token();
        count++;
    } while(*token == ',');
    count--;
    
    // Agora, empilha em local_var_stack na ordem invertida
    for(; count>- 0; count--){
        i.value = temp[count];
        i.var_type = ARG;
        local_push(i);
    }
}

// Obtém parâmetros da função
void get_params(void){
    
}

// 
void func_ret(void){
    
}

// 
void local_push(struct var_type i){
    
}

//
int func_pop(void){
    
}

void func_push(int i){
    
}

// 
void assign_var(char* var_name, int value){
    
}

/* 
 *  */
int fin_var(char* s){
    
}

/* 
 *  */
int is_var(char* s){
    
}

// 
void exec_if(void){
    
}

//
void exec_while(void){
    
}

//
void exec_do(void){
    
}

// 
void find_eob(void){
    
}

//
void exec_for(void){
    
}
