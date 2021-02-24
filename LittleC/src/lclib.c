/***********************************************************
 * Github: https://github.com/Bacagine/C_Completo_e_Total  *
 *                                                         *
 * lclib.c: Funções da biblioteca interna                  *
 *                                                         *
 * Este código fonte pode ser encontrado no livro:         *
 * "C Completo e Total" Parte 5                            *
 *                                                         *
 * Este código foi modificado por:                         *
 * Gustavo Bacagine <gustavo.bacagine@protonmail.com>      *
 *                                                         *
 * Data: 07/02/2020                                        *
 ***********************************************************/

/*#include <conio.h> Elimine isto se seu
compilador não suporta
este arquivo de cabeçalho */

#include <stdio.h>
#include <stdlib.h>
// Adicione mais por conta própia aqui

extern char *prog; /* Aponta para a posição corrente no
                    * programa */
extern char token[80]; /* Mantém a representação string do
                        * token */
extern char token_type; // Contém o tipo do token
extern char tok;       // Mantém a representação interna do token

enum tok_types{ DELIMITER, IDENTIFIER, NUMBER, KEYWORD, TEMP,
                STRING, BLOCK };

/* Estas são as constantes usadas para chamar sntx_err() quando
 * ocorre um erro de sintaxe. Adicione mais, se desejar.
 * NOTA: SYNTAX é uma mensagem genérica de erro usada quando
 * nenhuma outra parece apropiada. */
enum error_msg{ SYNTAX, UNBAL_PARENS, NO_EXP, EQUALS_EXPECTED,
                NOT_VAR, PARAM_ERR, SEMI_EXPECTED, NEST_FUNC,
                RET_NOCALL, PAREN_EXPECTED, WHILE_EXPECTED,
                QUOTE_EXPECTED, NOT_TEMP, TOO_MANY_LVARS };

int get_token(void);
void sntx_err(int error), eval_exp(int *result);
void putback(void);

/* Obtém um caractere do console. (Use getchar()
 * se o seu compilador não suportar getche(). ) */
int call_getchar(void){ // call_getche
    char ch;
    ch = getchar();
    while(*prog != ')') prog++;
    prog++; // Avança até o fim da linha
    
    return ch;
}

// Exibe um caractere na tela
int call_putch(void){
    int value;
    
    eval_exp(&value);
    printf("%c", value);
    
    return value;
}

// Chama puts()
int call_puts(void){
    get_token();
    if(*token != '(') sntx_err(QUOTE_EXPECTED);
    get_token();
    if(token_type != STRING) sntx_err(PAREN_EXPECTED);
    puts(token);
    get_token();
    if(*token != ')') sntx_err(SEMI_EXPECTED);
    putback();
    
    return 0;
}

// Uma função embutida de saída para o console
int print(void){
    int i;
    
    get_token();
    if(*token != '(') sntx_err(PAREN_EXPECTED);
    
    get_token();
    if(token_type == STRING){ // Exibe uma string
        printf("%s", token);
    }
    else{ // Exibe um número
        putback();
        eval_exp(&i);
        printf("%d", i);
    }
    
    get_token();
    
    if(*token != ')') sntx_err(PAREN_EXPECTED);
    
    get_token();
    if(*token != ';') sntx_err(SEMI_EXPECTED);
    putback();
    
    return 0;
}

// Lê um inteiro do teclado
int getnum(void){
    char s[80];
    
    gets(s);
    while(*prog != ')') prog++;
    prog++; // Avança até o fim da linha
    
    return atoi(s);
}
