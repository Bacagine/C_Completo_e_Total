/*********************************************************************
 * Github: https://github.com/Bacagine/C_Completo_e_Total            *
 *                                                                   *
 * parser.c: Analisador recursivo descendente de expressões inteiras *
 * que pode incluir variáveis e chamadas de funções                  *
 *                                                                   *
 * Este código fonte pode ser encontrado no livro:                   *
 * "C Completo e Total" Parte 5                                      *
 *                                                                   *
 * Este código foi modificado por:                                   *
 * Gustavo Bacagine <gustavo.bacagine@protonmail.com>                *
 *                                                                   *
 * Data: 07/02/2020                                                  *
 *********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

#define NUM_FUNC        100
#define NUM_GLOBAL_VARS 100
#define NUM_LOCAL_VARS  200
#define ID_LEN          31
#define FUNC_CALLS      31
#define PROG_SIZE       10000
#define FOR_NEST        31


enum tok_types{ DELTIMER, IDENTIFIER, NUMBER, KEYWORD, TEMP,
                STRING, BLOCK };

enum tokens{ ARG, CHAR, INT, IF, ELSE, FOR, DO, WHILE,
             SWITCH, RETURN, EOL, FINISHED, END };

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

extern char *prog;      // Posição corrente no código-fonte
extern char *p_buf;    /* Aponta para o início da
                        * área de carga do programa */
extern jmp_buf e_buf; // Mantém ambiente para longjmp()

/* Uma matriz destas estruturas manterá a informação
 * associada com as variáveis globais */
extern struct var_type{
    char var_name[32];
    enum variable_type var_type;
    int value;
} global_vars[NUM_GLOBAL_VARS];

// Esta é a pilha de chamadas de função
extern struct func_type{
    char func_name[32];
    char *loc; /* Posição do ponto de entrada da função no
                * arquivo */
} func_stack[NUM_FUNC];

// Tabela de palavras reservadas
extern struct commands{
    char command[20];
    char tok;
} table[];

/* Funções da "biblioteca padrão" são declaradas aqui para que
 * possam ser colocadas na tabela interna de funções que segue */
int call_getche(void), call_putch(void);
int call_puts(void), print(void), getnum(void);

struct intern_func_type{
    char *f_name; // Nome da função
    int (*p)();  // Ponteiro para a função
} intern_func[] = {
    { "getche", call_getche },
    { "putch", call_putch   },
    { "puts",  call_puts    },
    { "print", print        },
    { "getnum", getnum      },
    { "", 0                 } // NULL termina a lista
    
};

extern char token[80];   // Representação string do token
extern char token_type; // Contém o tipo do token
extern char tok;       // Representação interna do token

extern int ret_value; // Valor de retorno de função

void eval_exp(int *value), eval_exp1(int *value);
void eval_exp2(int *value);
void eval_exp3(int *value), eval_exp4(int *value);
void eval_exp5(int *value), atom(int *value);
void eval_exp0(int *value);
void sntx_err(int error), putback(void);
void assingn_var(char *var_name, int value);
int isdelim(char c), look_up(char *s), iswitch(char c);
int find_var(char *s), get_token(void);
int internal_func(char *s);
int is_var(char *s);
int *find_func(char *name);
void call(void);

// Ponto de entrada do analisador
void eval_exp(int *value){
    get_token();
    if(!*token){
        sntx_err(NO_EXP);
        return;
    }
    if(*token == ';'){
        *value = 0; // Expressão vazia
        return;
    }
    eval_exp(value);
    putback(); // Devolve último token lido para a entrada
}

// Processa uma expressão de atribuição
void eval_exp0(int *value){
    char temp[ID_LEN]; /* Guarda nome da variável que está
                        * recebendo a atribuição */
    register int temp_tok;
    
    if(token_type == IDENTIFIER){
        if(is_var(token)){ /* Se é uma variável 
                            * veja se é uma atribuição */
            strcpy(temp, token);
            temp_tok = token_type;
            get_token();
            if(*token == '='){ // É uma atribuição
                get_token();
                eval_exp0(value); // Obtém valor a atribuir
                assingn_var(temp, *value); // Atribui o valor
                return;
            }
            else{ // Não é uma atribuição
                putback(); // Restaura token original
                strcpy(token, temp);
                token_type = temp_tok;
            }
        }
    }
    eval_exp1(value);
}

/* Esta matriz é usada por eval_exp1(). Como alguns
 * compiladores não permitem inicializar uma matriz
 * dentro de uma função, ela é definida como uma
 * variável global */
char relops[7] = {
    LT, LE, GT, GE, EQ, NE, 0
};

// Processa operadores relacionais
void eval_exp1(int *value){
    int partial_value;
    register char op;
    
    eval_exp2(value);
    op = *token;
    if(strchr(relops, op)){
        get_token();
        eval_exp2(&partial_value);
        switch(op){ // Efetua a operação relacional
            case LT:
                *value = *value < partial_value;
                break;
            case LE:
                *value = *value <= partial_value;
                break;
            case GT:
                *value = *value > partial_value;
                break;
            case GR:
                *value = *value >= partial_value;
                break;
            case EQ:
                *value = *value == partial_value;
                break;
            case NE:
                *value = *value != partial_value;
                break;
        }
    }
}

// Soma ou subtrai dois termos
void eval_exp2(int *value){
    register char op;
    int partial_value;
    
    eval_exp3(value);
    while((op = *token) == '+' || op == '-'){
        get_token();
        eval_exp3(&partial_value);
        switch(op){ // Soma ou subtrai
            case '-':
                *value = *value - partial_value;
                break;
            case '+':
                *value = *value + partial_value;
                break;
        }
    }
}

// Múltiplica ou divide dois fatores
void eval_exp3(int *value){
    register char op;
    int partial_value, t;
    
    eval_exp4(value);
    while((op = *token) == '*' || op == '/' || op == '%'){
        get_token();
        eval_exp4(&partial_value);
        switch(op){
            case '*':
                *value = *value * partial_value;
                break;
            case '/':
                *value = *value / partial_value;
                break;
            case '%':
                t = (*value) / partial_value;
                *value = *value - (t * partial_value);
                break;
        }
    }
}

// É um + ou - unário
void eval_exp4(int *value){
    register char op;
    
    op = '\0';
    if(*token == '+' || *token == '-'){
        op = *token;
        get_token();
    }
    eval_exp5(value);
    if(op){
        if(op == '-'){
            *value = - (*value);
        }
    }
}

// Processa expressões com parênteses
void eval_exp5(int *value){
    if((*token == '(')){
        get_token();
        eval_exp0(value); // Obtém subexpressão
        if(*token != ')') sntx_err(PAREN_EXPECTED);
        get_token();
    }
    else{
        atom(value);
    }
}

// Acha valor de número, variável ou função.
void atom(int *value){
    int i;
    
    switch(token_type){
        case IDENTIFIER:
            i = internal_func(token);
            if(i != -1){ // Chama função da "biblioteca padrão"
                *value = (*intern_func[i].p)();
            }
            else if(find_func(token)){ // Chama função definida pelo usuário
                call();
                *value = ret_value;
            }
            else{
                *value = find_var(token); // Obtém valor da variável
                get_token();
                return;
            }
        case NUMBER: // É uma constante numérica
             *value = atoi(token);
             get_token();
             return;
        case DELTIMER: // Veja se é uma constante caractere
            if(*token == '\''){
                *value = *prog;
                prog++;
                if(*prog != '\'') sntx_err(QUOTE_EXPECTED);
                prog++;
                get_token();
            }
            return;
        default:
            if(*token == ')') return; // Processa expressão vazia
            else sntx_err(SYNTAX); // Erro de sintaxe
    }
}

// Exibe uma mensagem de erro
void sntx_err(int error){
    char *p, *temp;
    int linecount = 0;
    register int i;
    
    static char *e[] = {
        "erro de sintaxe",
        "parênteses desbalanceados",
        "falta uma expressão",
        "esperando sinal de igual",
        "não é uma variável",
        "erro de paramêtro",
        "esperando ponto-e-vírgula",
        "chaves desbalanceadas",
        "função não definida",
        "esperado identificador",
        "excessivas aninhadas de função",
        "return sem chamada",
        "esperado parênteses",
        "esperado while",
        "esperando fechas aspas",
        "não é uma string",
        "excessivas variáveis locais"
    };
    printf("%s", e[error]);
    p = p_buf;
    while(p != prog){ // Encontra linha do erro
        p++;
        if(*p == '\r'){
            linecount++;
        }
    }
    printf(" na linha %d\n", linecount);
    
    temp = p;
    for(i = 0; i < 20 && p > p_buf && *p != '\n'; i++, p--);
    for(i = 0; i < 30 && p <= temp; i++, p++) printf("%c", *p);
    
    longjmp(e_buf, 1); // Retorna para uma ponto seguro
}

// Obtém o token
int get_token(void){
    register char *temp;
    
    token_type = 0; tok = 0;
    
    temp = token;
    *temp = '\0';
    
    // Ignora espaço vazio
    while(iswitch(*prog) && *prog) ++prog;
    
    if(){
        
    }
    
    return ;
}

void putback(void){
    
}

void assingn_var(char *var_name, int value){
    
}

int isdelim(char c){
    
    return ;
}

int look_up(char *s){
    
    return ;
}

int iswitch(char c){
    
    return ;
}

int find_var(char *s){
    
    return ;
}


int internal_func(char *s){
    
    return ;
}

int is_var(char *s){
    
    return ;
}

int *find_func(char *name){
    
    return ;
}

void call(void){
    
}
