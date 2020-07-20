#include <stdio.h>
#include <stdlib.h>
#include <getsym.h>
extern TOKEN tok;
extern FILE *infile;
extern FILE *outfile;

void error(char *s);
void statement(void);
void outblock(void);
void expression(void);
void print_tok(void);
void condition(void);

typedef struct{
    int addr;
    char v[MAXIDLEN + 1];
}s_entry;

s_entry s_table[32];
