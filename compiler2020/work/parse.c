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
		int var_address = -1;
		for(i=0;i<S_TABLELEN;i++){
			if(strcmp(tok.charvalue,s_table[i].v) == 0){
				var_address = s_table[i].addr;
				break;
			}
		}
		if(var_address == -1){
			char output[MAXIDLEN+10];
			strcat(output,"not found ");
			error(strcat(output,tok.charvalue));
		}
		// printf("%d",var_address);
		getsym();
		print_tok();
		if(tok.value == BECOMES){
			getsym();
			expression(0);
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
				int flag = -1;
				for(i=0;i<S_TABLELEN;i++){
					if(strcmp(tok.charvalue,s_table[i].v) == 0){
						fprintf(outfile,"load r0,%d\n",s_table[i].addr);
						fprintf(outfile,"writed r0\n");
						flag = 1;
						break;
					}
				}
				if(flag == -1){
					char output[MAXIDLEN+10];
					strcat(output,"not found ");
					error(strcat(output,tok.charvalue));
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

// rnに計算結果を返す
void expression(int n){

	print_tok();
	
	term(n);
	if((tok.attr == SYMBOL && (tok.value == PLUS || tok.value == MINUS))){
		while((tok.attr == SYMBOL && (tok.value == PLUS || tok.value == MINUS))){
			if(tok.value == PLUS){
				fprintf(outfile,"push r%d\n",(n+1)%4);
				print_tok();
				getsym();
				term((n+1)%4);
				fprintf(outfile,"addr r%d,r%d\n",n,(n+1)%4);
				fprintf(outfile,"pop r%d\n",(n+1)%4);
			}else if(tok.value == MINUS){
				fprintf(outfile,"push r%d\n",(n+1)%4);
				print_tok();
				getsym();
				term((n+1)%4);
				fprintf(outfile,"subr r%d,r%d\n",n,(n+1)%4);
				fprintf(outfile,"pop r%d\n",(n+1)%4);
			}
		}
	}

}
// rnに返す
void term(int n){
	print_tok();
	factor(n);
	if((tok.attr == SYMBOL && tok.value == TIMES) || (tok.attr == RWORD && tok.value == DIV)){
		while((tok.attr == SYMBOL && tok.value == TIMES) || (tok.attr == RWORD && tok.value == DIV)){
			if(tok.value == TIMES){
				fprintf(outfile,"push r%d\n",(n+1)%4);	
				print_tok();
				getsym();
				factor((n+1)%4);
				fprintf(outfile,"mulr r%d,r%d\n",n,(n+1)%4);
				fprintf(outfile,"pop r%d\n",(n+1)%4);
			}
			else if(tok.value == DIV){
				fprintf(outfile,"push r%d\n",(n+1)%4);
				print_tok();
				getsym();
				factor((n+1)%4);
				fprintf(outfile,"divr r%d,r%d\n",n,(n+1)%4);
				fprintf(outfile,"pop r%d\n",(n+1)%4);
			}
		}
	}
}
// rnに返す
void factor(int n){

	bool minus_flag  = false;

	if(tok.attr == SYMBOL && tok.value == MINUS){
		minus_flag = true;
		print_tok();
		getsym();

	}


	if(tok.attr == IDENTIFIER){
		print_tok();
		// identifier read
		int i;
		int var_address = -1;
		for(i=0;i<S_TABLELEN;i++){
			if(strcmp(tok.charvalue,s_table[i].v) == 0){
				var_address = s_table[i].addr;
				break;
			}
		}
		if(var_address == -1){
			char temp[MAXIDLEN+1+14] = "not found var ";
			error(strcat(temp,tok.charvalue));
		}
		if(minus_flag == false){
			fprintf(outfile,"load r%d,%d\n",n,var_address);
			// fprintf(outfile,"push r2");
		}
		else if(minus_flag == true){
			fprintf(outfile,"load r%d,-%d\n",n,var_address);
			// fprintf(outfile,"push r2");
		}

	}
	else if(tok.attr == NUMBER){
		print_tok();
		if(minus_flag == false){
			fprintf(outfile,"loadi r%d,%d\n",n,tok.value);
		}
		else if(minus_flag == true){
			fprintf(outfile,"loadi r%d,%d\n",n,-tok.value);
		}
	}else if(tok.attr == SYMBOL && tok.value == LPAREN){
		print_tok();
		getsym();
		expression(n);
		if(minus_flag == true){
			fprintf(outfile,"muli r%d -1\n",n);
		}
		if(tok.attr == SYMBOL && tok.value == RPAREN){
			print_tok();
		}
	}

	getsym();

}

void outblock(void){
	int address = 0;
	print_tok();
	if(tok.attr == RWORD && tok.value == VAR){
		do{
			getsym();
			print_tok();
			strcpy(s_table[address].v,tok.charvalue);
			s_table[address].addr = address + VAR_BASEADDR;
			address++;
			getsym();
			print_tok();
		}while(tok.value == COMMA);
		if(tok.value == SEMICOLON){
			getsym();
			statement();
		}
	}
	print_symboltable();
	fprintf(outfile,"halt\n");
}

void condition(int index){
	fprintf(outfile,"push r0\n");
	expression(0);
	fprintf(outfile,"push r1\n");
	fprintf(outfile,"loadr r1,r0\n");
	print_tok();
	if(tok.attr == SYMBOL){
		if(tok.value == EQL){
			getsym();
			// print_tok();
			expression(0);
			fprintf(outfile,"cmpr r1,r0\n");
			fprintf(outfile,"pop r1\n");
			fprintf(outfile,"pop r0\n");
			fprintf(outfile,"jnz L%d\n",index);
		}else if(tok.value == NOTEQL){
			getsym();
			// print_tok();
			expression(0);
			fprintf(outfile,"cmpr r1,r0\n");
			fprintf(outfile,"pop r1\n");
			fprintf(outfile,"pop r0\n");
			fprintf(outfile,"jz L%d\n",index);
		}else if(tok.value == LESSTHAN){
			getsym();
			// print_tok();
			expression(0);
			fprintf(outfile,"cmpr r1,r0\n");
			fprintf(outfile,"pop r1\n");
			fprintf(outfile,"pop r0\n");
			fprintf(outfile,"jge L%d\n",index);
		}else if(tok.value == GRTRTHAN){
			getsym();
			// print_tok();
			expression(0);
			fprintf(outfile,"cmpr r1,r0\n");
			fprintf(outfile,"pop r1\n");
			fprintf(outfile,"pop r0\n");
			fprintf(outfile,"jle L%d\n",index);
		}else if(tok.value == LESSEQL){
			getsym();
			// print_tok();
			expression(0);
			fprintf(outfile,"cmpr r1,r0\n");
			fprintf(outfile,"pop r1\n");
			fprintf(outfile,"pop r0\n");
			fprintf(outfile,"jgt L%d\n",index);
		}else if(tok.value == GRTREQL){
			getsym();
			// print_tok();
			expression(0);
			fprintf(outfile,"cmpr r1,r0\n");
			fprintf(outfile,"pop r1\n");
			fprintf(outfile,"pop r0\n");
			fprintf(outfile,"jlt L%d\n",index);
		}else{
			error("syntax error in condition");
		}
	}else{
		error("syntax error in condition");
	}

}