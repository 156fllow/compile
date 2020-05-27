#include <stdio.h>
#include <stdlib.h>
#include <getsym.h>
extern TOKEN tok;
extern FILE *infile;
extern FILE *outfile;

void error(char *s);
void statement(void);

void compiler(void){
	init_getsym();

	getsym();

	if (tok.attr == RWORD && tok.value == PROGRAM){

		getsym();

		if (tok.attr == IDENTIFIER){

			getsym();

			if (tok.attr == SYMBOL && tok.value == SEMICOLON){

				getsym();

				statement();

				if (tok.attr == SYMBOL && tok.value == PERIOD){
					fprintf(stderr, "Parsing Completed. No errors found.\n");
				} else error("At the end, a period is required.");
			}else error("After program name, a semicolon is needed.");
		}else error("Program identifier is needed.");
	}else error("At the first, program declaration is required.");
}

void error(char *s){
	fprintf(stderr, "%s\n", s);
	exit(1);
}

//
// Parser
//

void statement(void){

}

