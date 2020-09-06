#include <stdio.h>
#include <stdlib.h>
#include <getsym.h>
#include <string.h>
#include <stdbool.h>

#define S_TABLELEN 32
#define VAR_BASEADDR 1000
#define EXPRESSION_BASEADDR 1100
#define TERM_BASEADDR 1200
#define FACTOR_BASEADDR 1300

extern TOKEN tok;
extern FILE *infile;
extern FILE *outfile;

void error(char *s);
void statement(void);
void outblock(void);
int expression(void);
void print_tok(void);
void condition(int);
void print_symboltable(void);
int term(void);
int factor(void);

typedef struct{
    int addr;
    char v[MAXIDLEN + 1];
}s_entry;

s_entry s_table[S_TABLELEN];

typedef struct{
    int label_count;
    int value;
}v_entry;

v_entry v_table[S_TABLELEN];
int v_label = 0;

int jmp_label = 0;



