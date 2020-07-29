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

void print_symboltable(){
	printf("value\taddress\n");
	int i=0;
	for(i=0;i<S_TABLELEN;i++){
		printf("%s\t%d\n",s_table[i].v,s_table[i].addr);
	}
}

void statement(void){
	print_tok();
	if(tok.attr == IDENTIFIER){
		// identifier read
		int i;
		int var_address;
		for(i=0;i<S_TABLELEN;i++){
			if(strcmp(tok.charvalue,s_table[i].v) == 0){
				var_address = s_table[i].addr;
				break;
			}
		}
		printf("%d",var_address);
		getsym();
		print_tok();
		if(tok.value == BECOMES){
			getsym();
			expression();
			fprintf(outfile,"store r0,%d\n",var_address);
		}else{
			error("NOT FOUND BECOMS");
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
			condition(jmp_label);
			if(tok.value == THEN){
				getsym();
				statement();
				if(tok.value == ELSE){
					fprintf(outfile,"jmp L%d\n",jmp_label+1);
				}
				fprintf(outfile,"L%d:\n",jmp_label);
				jmp_label++;
				if(tok.value == ELSE){
					getsym();
					statement();
					fprintf(outfile,"L%d:\n",jmp_label);
					jmp_label++;
				}
			}else{
				error("Not found THEN");
			}
		}
		else if(tok.value == WHILE){
			getsym();
			fprintf(outfile,"L%d:\n",jmp_label);
			jmp_label++;
			int temp_jmp_label = jmp_label;
			condition(temp_jmp_label);
			jmp_label++;
			print_tok();
			if(tok.value == DO){
				getsym();
				statement();
				fprintf(outfile,"jmp L%d\n",temp_jmp_label-1);
			}
			fprintf(outfile,"L%d:\n",temp_jmp_label);
			
		}
		else if(tok.value == WRITE){
			getsym();
			print_tok();
			while(tok.attr == IDENTIFIER){
				int i;
				for(i=0;i<S_TABLELEN;i++){
					if(strcmp(tok.charvalue,s_table[i].v) == 0){
						fprintf(outfile,"load r0,%d\n",s_table[i].addr);
						fprintf(outfile,"writed r0\n");
						break;
					}
				}

				getsym();
				print_tok();
				if(tok.value == COMMA){
					fprintf(outfile,"loadi r0,' '\n");
					fprintf(outfile,"writec r0\n");
					getsym();
					print_tok();
				}
			}
			fprintf(outfile,"loadi r0,'\\n'\n");
			fprintf(outfile,"writec r0\n");
		}
	}else{
		error("syntax error in statement");
	}
}

// r0に計算結果を返す
void expression(void){

	print_tok();	
	if(tok.attr == NUMBER){
		fprintf(outfile,"loadi r0,%d\n",tok.value);
	}
	else if(tok.attr == IDENTIFIER){
		int i;
		for(i=0;i<S_TABLELEN;i++){
			if(strcmp(tok.charvalue,s_table[i].v) == 0){
				fprintf(outfile,"load r0,%d\n",s_table[i].addr);
				break;
			}
		}
	}

	getsym();
	

	if(tok.attr == RWORD || tok.attr == SYMBOL){
		
		if(tok.value == PLUS){
			getsym();
			if(tok.attr == NUMBER){
				fprintf(outfile,"addi r0,%d\n",tok.value);

			}else if(tok.attr == IDENTIFIER){
				int i;
				for(i=0;i<S_TABLELEN;i++){
					if(strcmp(tok.charvalue,s_table[i].v) == 0){
						fprintf(outfile,"load r1,%d\n",s_table[i].addr);
						break;
					}
				}
				fprintf(outfile,"addr r0,r1\n");

			}else{
				error("SYNTAX ERROR IN EXPRESSION");
			}
			getsym();

		}else if(tok.value == MINUS){
			getsym();
			if(tok.attr == NUMBER){
				fprintf(outfile,"subi r0,%d\n",tok.value);

			}else if(tok.attr == IDENTIFIER){
				int i;
				for(i=0;i<S_TABLELEN;i++){
					if(strcmp(tok.charvalue,s_table[i].v) == 0){
						fprintf(outfile,"load r1,%d\n",s_table[i].addr);
						break;
					}
				}
				fprintf(outfile,"subr r0,r1\n");

			}else{
				error("SYNTAX ERROR IN EXPRESSION");
			}
			getsym();

		}else if(tok.value == TIMES){
			getsym();
			
			if(tok.attr == NUMBER){
				fprintf(outfile,"muli r0,%d\n",tok.value);
			}else if(tok.attr == IDENTIFIER){
				int i;
				for(i=0;i<S_TABLELEN;i++){
					if(strcmp(tok.charvalue,s_table[i].v) == 0){
						fprintf(outfile,"load r1,%d\n",s_table[i].addr);
						break;
					}
				}
				fprintf(outfile,"mulr r0,r1\n");
			}else{
				error("SYNTAX ERROR IN EXPRESSION");
			}
			getsym();

		}else if(tok.value == DIV){
			getsym();
			if(tok.attr == NUMBER){
				fprintf(outfile,"divi r0,%d\n",tok.value);

			}else if(tok.attr == IDENTIFIER){
				int i;
				for(i=0;i<S_TABLELEN;i++){
					if(strcmp(tok.charvalue,s_table[i].v) == 0){
						fprintf(outfile,"load r1,%d\n",s_table[i].addr);
						break;
					}
				}
				fprintf(outfile,"divr r0,r1\n");
			}else{
				error("SYNTAX ERROR IN EXPRESSION");
			}
			getsym();
		}
	
	}
}

void outblock(void){
	int address = 0;
	print_tok();
	if(tok.attr == RWORD && tok.value == VAR){
		getsym();
		print_tok();
		strcpy(s_table[address].v,tok.charvalue);
		s_table[address].addr = address;
		address++;
		getsym();
		while(tok.value == COMMA){
			print_tok();
			getsym();
			strcpy(s_table[address].v,tok.charvalue);
			s_table[address].addr = address;
			address++;
		}
		print_tok();
		if(tok.value == SEMICOLON){
			getsym();
			statement();
		}
	}
	fprintf(outfile,"halt\n");
}

void condition(int index){
	expression();
	fprintf(outfile,"loadr r1,r0\n");
	print_tok();
	if(tok.attr == SYMBOL){
		if(tok.value == EQL){
			getsym();
			// print_tok();
			expression();
			fprintf(outfile,"cmpr r1,r0\n");
			fprintf(outfile,"jnz L%d\n",index);
		}else if(tok.value == NOTEQL){
			getsym();
			// print_tok();
			expression();
			fprintf(outfile,"cmpr r1,r0\n");
			fprintf(outfile,"jz L%d\n",index);
		}else if(tok.value == LESSTHAN){
			getsym();
			// print_tok();
			expression();
			fprintf(outfile,"cmpr r1,r0\n");
			fprintf(outfile,"jge L%d\n",index);
		}else if(tok.value == GRTRTHAN){
			getsym();
			// print_tok();
			expression();
			fprintf(outfile,"cmpr r1,r0\n");
			fprintf(outfile,"jle L%d\n",index);
		}else if(tok.value == LESSEQL){
			getsym();
			// print_tok();
			expression();
			fprintf(outfile,"cmpr r1,r0\n");
			fprintf(outfile,"jgt L%d\n",index);
		}else if(tok.value == GRTREQL){
			getsym();
			// print_tok();
			expression();
			fprintf(outfile,"cmpr r1,r0\n");
			fprintf(outfile,"jlt L%d\n",index);
		}else{
			error("syntax error in condition");
		}
	}else{
		error("syntax error in condition");
	}
}