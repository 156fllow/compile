#include <stdio.h>
#include <stdlib.h>
#include <getsym.h>
extern TOKEN tok;
extern FILE *infile;
extern FILE *outfile;

void error(char *s);
void statement(void);

void compiler(void)
{
	init_getsym();

	getsym();

	if (tok.attr == RWORD && tok.value == PROGRAM)
	{

		getsym();

		if (tok.attr == IDENTIFIER)
		{

			getsym();

			if (tok.attr == SYMBOL && tok.value == SEMICOLON)
			{

				getsym();

				statement();

				if (tok.attr == SYMBOL && tok.value == PERIOD)
				{
					fprintf(stderr, "Parsing Completed. No errors found.\n");
				}
				else
					error("At the end, a period is required.");
			}
			else
				error("After program name, a semicolon is needed.");
		}
		else
			error("Program identifier is needed.");
	}
	else
		error("At the first, program declaration is required.");
}

void error(char *s)
{
	fprintf(stderr, "%s\n", s);
	exit(1);
}

//
// Parser
//

void print_tok()
{
	printf("attr=%d\tvalue=%d\tcharvalue=%s\tsline=%d\n", tok.attr, tok.value, tok.charvalue, tok.sline);
}

void statement(void)
{
	print_tok();

	if (tok.attr == NUMBER)
	{
		getsym();

		if (tok.attr == SYMBOL || tok.attr == RWORD)
		{
			switch (tok.value)
			{
			case PLUS:
				print_tok();
				getsym();
				print_tok();
				break;
			case MINUS:
				print_tok();
				getsym();
				print_tok();
				break;
			case TIMES:
				print_tok();
				getsym();
				print_tok();
				break;
			case DIV:
				print_tok();
				getsym();
				print_tok();
				break;
			}
			getsym();
		}
	}
	else if (tok.attr == RWORD)
	{
		if (tok.value == BEGIN)
		{
			getsym();
			statement();
			print_tok();
		}
	}

	if (tok.value == SEMICOLON)
	{
		getsym();
		statement();
	}

	if (tok.value == END)
	{
		getsym();
	}
}
