#include <stdio.h>
#include <stdlib.h>
#include <getsym.h>
#include <string.h>

#define S_TABLELEN 32

extern TOKEN tok;
extern FILE *infile;
extern FILE *outfile;

void error(char *s);
void statement(void);
void outblock(void);
void expression(void);
void print_tok(void);
void condition(int);
void print_symboltable(void);

typedef struct{
    int addr;
    char v[MAXIDLEN + 1];
}s_entry;

s_entry s_table[S_TABLELEN];

int jmp_label = 0;
