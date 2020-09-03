#include <stdio.h>
#include <stdlib.h>
#include <getsym.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define S_TABLELEN 32
#define VAR_BASEADDR 1000
#define EXPRESSION_BASEADDR 1100
#define TERM_BASEADDR 1200
#define FACTOR_BASEADDR 1300
#define STACK_SIZE 1000

#define ENDMARKER '$'

extern TOKEN tok;
extern FILE *infile;
extern FILE *outfile;

void error(char *s);
void statement(void);
void outblock(void);
int expression();
void print_tok(void);
void condition(int);
void print_symboltable(void);
void term(int);
void factor(int);
int opeEval(char*,char*);
bool checkDigit(char*);
void inblock(void);

void stack_initialize(void);
void i_stack_initialize(void);
void o_stack_initialize(void);
void push(char*);
char* pop(void);
void i_push(char*);
char* i_pop(void);
void o_push(char*);
char* o_pop(void);



typedef struct{
    int addr;
    char v[MAXIDLEN + 1];
}s_entry;

s_entry s_table[S_TABLELEN];

// local var control
typedef struct{
    int head;
    s_entry table[S_TABLELEN];
}s_entry_p;

s_entry_p s_table_procedure[S_TABLELEN];

int s_table_count = 0;
// --------------------------



typedef struct{
    int label_count;
    int value;
}v_entry;

typedef struct{
    int head;
    char v[STACK_SIZE][MAXIDLEN + 1];
}p_entry;

v_entry v_table[S_TABLELEN];

int v_label = 0;

p_entry p_table;



typedef struct{
    int head;
    char data[STACK_SIZE][MAXIDLEN + 1];
}Stack;

typedef struct{
    int head;
    char data[500][50];
}intermediate_stack;

typedef struct{
    int head;
    char data[100][10];
}operand_stack;

Stack stack;

intermediate_stack i_stack;

operand_stack o_stack;

int jmp_label = 0;



