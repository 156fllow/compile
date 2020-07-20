#include "parse.h"

void compiler(void)
{
	init_getsym();

	getsym();
	print_tok();

	if (tok.attr == RWORD && tok.value == PROGRAM)
	{

		getsym();
		print_tok();

		if (tok.attr == IDENTIFIER)
		{

			getsym();
			print_tok();

			if (tok.attr == SYMBOL && tok.value == SEMICOLON)
			{

				getsym();

				outblock();

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

// void statement(void)
// {
// 	print_tok();
// 
// 	if (tok.attr == NUMBER)
// 	{
// 		fprintf(outfile, "loadi r0,%d\n", tok.value);
// 		getsym();
// 		print_tok();
// 
// 		if (tok.attr == SYMBOL || tok.attr == RWORD)
// 		{
// 
// 			switch (tok.value)
// 			{
// 			case PLUS:
// 				getsym();
// 				print_tok();
// 				fprintf(outfile, "addi r0,%d\n", tok.value);
// 				break;
// 			case MINUS:
// 				getsym();
// 				print_tok();
// 				fprintf(outfile, "subi r0,%d\n", tok.value);
// 				break;
// 			case TIMES:
// 				getsym();
// 				print_tok();
// 				fprintf(outfile, "muli r0,%d\n", tok.value);
// 				break;
// 			case DIV:
// 				getsym();
// 				print_tok();
// 				fprintf(outfile, "divi r0,%d\n", tok.value);
// 				break;
// 			}
// 			fprintf(outfile, "writed r0\n");
// 			fprintf(outfile, "loadi r1,'\\n'\n");
// 			fprintf(outfile, "writec r1\n");
// 			getsym();
// 			print_tok();
// 		}
// 	}
// 	else if (tok.attr == RWORD)
// 	{
// 		if (tok.value == BEGIN)
// 		{
// 			getsym();
// 			statement();
// 			while(tok.value == SEMICOLON)
// 			{
// 				getsym();
// 				statement();
// 			}
// 			if (tok.value == END)
// 			{
// 				getsym();
// 			}
// 			else
// 			{
// 				error("Not found END");
// 			}
// 		}
// 	}
// 
// }

void statement(void){
	print_tok();
	if(tok.attr == IDENTIFIER){
		getsym();
		if(tok.value == BECOMES){
			print_tok();
			getsym();
			expression();
		}

	}else if(tok.attr == RWORD){
		if(tok.value == BEGIN)
		{
			getsym();
			statement();
			while(tok.value == SEMICOLON)
			{
				getsym();
				statement();
			}
			if (tok.value == END)
			{
				getsym();
			}
			else
			{
				error("Not found END");
			}
		}
		else if (tok.value == IF){
			getsym();
			condition();
			if(tok.value == THEN){
				getsym();
				statement();
				if(tok.value == ELSE){
					getsym();
					statement();
				}
			}else{
				error("Not found THEN");
			}
		}
		else if(tok.value == WHILE){
			getsym();
			condition();
			if(tok.value == DO){
				getsym();
				statement();
			}
		}
		else if(tok.value == WRITE){
			getsym();
			print_tok();
			while(tok.attr == IDENTIFIER){
				getsym();
				print_tok();
				if(tok.value == COMMA){
					getsym();
					print_tok();
				}
			}
		}
	}else{
		error("syntax error in statement");
	}
}

void expression(void){
	print_tok();
	if(tok.attr == NUMBER || tok.attr == IDENTIFIER){
		getsym();
		if(tok.value == PLUS){
			print_tok();
			getsym();
			if(tok.attr == NUMBER || tok.attr == IDENTIFIER){
				print_tok();
				getsym();
			}
		}
		else if(tok.value == MINUS){
			print_tok();
			getsym();
			if(tok.attr == NUMBER || tok.attr == IDENTIFIER){
				print_tok();
				getsym();
			}
		}
		else if(tok.value == TIMES){
			print_tok();
			getsym();
			if(tok.attr == NUMBER || tok.attr == IDENTIFIER){
				print_tok();
				getsym();
			}
		}
		else if(tok.value == DIV){
			print_tok();
			getsym();
			if(tok.attr == NUMBER || tok.attr == IDENTIFIER){
				print_tok();
				getsym();
			}
		}
	}
}

void outblock(void){
	print_tok();
	if(tok.attr == RWORD && tok.value == VAR){
		getsym();
		print_tok();
		getsym();
		while(tok.value == COMMA){
			print_tok();
			getsym();
		}
		print_tok();
		if(tok.value == SEMICOLON){
			getsym();
			statement();
		}
	}
}

void condition(void){
	expression();
	print_tok();
	if(tok.attr == SYMBOL){
		if(tok.value == EQL){
			getsym();
			// print_tok();
		}else if(tok.value == NOTEQL){
			getsym();
			// print_tok();
		}else if(tok.value == LESSTHAN){
			getsym();
			// print_tok();
		}else if(tok.value == GRTRTHAN){
			getsym();
			// print_tok();
		}else if(tok.value == LESSEQL){
			getsym();
			// print_tok();
		}else if(tok.value == GRTREQL){
			getsym();
			// print_tok();
		}else{
			error("syntax error in condition");
		}
		expression();
	}else{
		error("syntax error in condition");
	}
}