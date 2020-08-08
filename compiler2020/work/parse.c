#include "parse.h"

void compiler(void)
{
	init_getsym();
	stack_initialize();

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
	printf("\nvalue\taddress\n");
	int i=0;
	for(i=0;i<S_TABLELEN;i++){
		
		if(strcmp(s_table[i].v,"") == 0){
			continue;
		}else{
			printf("%s\t%d\n",s_table[i].v,s_table[i].addr);
		}
	}
}

void print_intermediate(){
	int i=0;
	for(i=0;i<i_stack.head;i++){
		printf("%s\n",i_stack.data[i]);
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
	i_stack_initialize();
	o_stack_initialize();
	
	o_push("$");
	push("$");

	int label_num = 1;

	if(tok.attr == SYMBOL && tok.value == MINUS){
		o_push("!");
		push("!");
		getsym();
		print_tok();
	}

	do{
		if(tok.attr == IDENTIFIER){
			push(tok.charvalue);
			getsym();
			print_tok();

		}else if(tok.attr == NUMBER){
			push(tok.charvalue);
			getsym();
			print_tok();

		}else if(tok.attr == RWORD){
			if(tok.value == DIV){
				while(true){
					if(opeEval(o_stack.data[o_stack.head-1],tok.charvalue) == 1){
						char t3[MAXIDLEN+1];
						char t2[MAXIDLEN+1];
						char t1[MAXIDLEN+1];
						strcpy(t3,pop());
						strcpy(t2,pop());
						strcpy(t1,pop());
						o_pop();
						char intermediate_temp[50]; 
						sprintf(intermediate_temp,"%s,%s,%s,T%d",t2,t1,t3,label_num);
						i_push(intermediate_temp);
						char t_push[MAXIDLEN + 1];
						sprintf(t_push,"T%d",label_num);
						push(t_push);
						label_num++;
						print_tok();
					}else{
						o_push("div");
						push("div");
						break;
					}
				}
				getsym();
				print_tok();
				// if(tok.value == MINUS){
				// 	if(opeEval(o_stack.data[o_stack.head-1],"!") == 1){

				// 	}
				// }
			}else{
				while(true){
					if(opeEval(o_stack.data[o_stack.head-1],"$") == 1){
						char t3[MAXIDLEN+1];
						char t2[MAXIDLEN+1];
						char t1[MAXIDLEN+1];
						strcpy(t3,pop());
						strcpy(t2,pop());
						strcpy(t1,pop());
						o_pop();
						char intermediate_temp[50]; 
						sprintf(intermediate_temp,"%s,%s,%s,T%d",t2,t1,t3,label_num);
						i_push(intermediate_temp);
						char t_push[MAXIDLEN + 1];
						sprintf(t_push,"T%d",label_num);
						push(t_push);
						label_num++;
						print_tok();
					}
					else if(opeEval(o_stack.data[o_stack.head-1],"$") == 0){
						goto E;
					}
				}
				
			}

		}else if(tok.attr == SYMBOL){
			if(tok.value == PLUS){
				while(true){
					if(opeEval(o_stack.data[o_stack.head-1],tok.charvalue) == 1){
						char t3[MAXIDLEN+1];
						char t2[MAXIDLEN+1];
						char t1[MAXIDLEN+1];
						strcpy(t3,pop());
						strcpy(t2,pop());
						strcpy(t1,pop());
						o_pop();
						char intermediate_temp[50]; 
						sprintf(intermediate_temp,"%s,%s,%s,T%d",t2,t1,t3,label_num);
						i_push(intermediate_temp);
						char t_push[MAXIDLEN + 1];
						sprintf(t_push,"T%d",label_num);
						push(t_push);
						label_num++;
						print_tok();
					}else{
						o_push("+");
						push("+");
						break;
					}
				}
			}else if(tok.value == MINUS){
				while(true){
					if(opeEval(o_stack.data[o_stack.head-1],tok.charvalue) == 1){
						char t3[MAXIDLEN+1];
						char t2[MAXIDLEN+1];
						char t1[MAXIDLEN+1];
						strcpy(t3,pop());
						strcpy(t2,pop());
						strcpy(t1,pop());
						o_pop();
						char intermediate_temp[50]; 
						sprintf(intermediate_temp,"%s,%s,%s,T%d",t2,t1,t3,label_num);
						i_push(intermediate_temp);
						char t_push[MAXIDLEN + 1];
						sprintf(t_push,"T%d",label_num);
						push(t_push);
						label_num++;
						print_tok();
					}else{
						o_push("-");
						push("-");
						break;
					}
				}
			}else if(tok.value == TIMES){
				while(true){
					if(opeEval(o_stack.data[o_stack.head-1],tok.charvalue) == 1){
						char t3[MAXIDLEN+1];
						char t2[MAXIDLEN+1];
						char t1[MAXIDLEN+1];
						strcpy(t3,pop());
						strcpy(t2,pop());
						strcpy(t1,pop());
						o_pop();
						char intermediate_temp[50]; 
						sprintf(intermediate_temp,"%s,%s,%s,T%d",t2,t1,t3,label_num);
						i_push(intermediate_temp);
						char t_push[MAXIDLEN + 1];
						sprintf(t_push,"T%d",label_num);
						push(t_push);
						label_num++;
						print_tok();
					}else{
						o_push("*");
						push("*");
						break;
					}
				}
			}else if(tok.value == LPAREN){
				while(true){
					if(opeEval(o_stack.data[o_stack.head-1],tok.charvalue) == 1){
						char t3[MAXIDLEN+1];
						char t2[MAXIDLEN+1];
						char t1[MAXIDLEN+1];
						strcpy(t3,pop());
						strcpy(t2,pop());
						strcpy(t1,pop());
						o_pop();
						char intermediate_temp[50]; 
						sprintf(intermediate_temp,"%s,%s,%s,T%d",t2,t1,t3,label_num);
						i_push(intermediate_temp);
						char t_push[MAXIDLEN + 1];
						sprintf(t_push,"T%d",label_num);
						push(t_push);
						label_num++;
						print_tok();
					}else{
						o_push("(");
						push("(");
						break;
					}
				}
			}else if(tok.value == RPAREN){
				while(true){
					if(opeEval(o_stack.data[o_stack.head-1],tok.charvalue) == 1){
						char t3[MAXIDLEN+1];
						char t2[MAXIDLEN+1];
						char t1[MAXIDLEN+1];
						strcpy(t3,pop());
						strcpy(t2,pop());
						strcpy(t1,pop());
						o_pop();
						char intermediate_temp[50]; 
						sprintf(intermediate_temp,"%s,%s,%s,T%d",t2,t1,t3,label_num);
						i_push(intermediate_temp);
						char t_push[MAXIDLEN + 1];
						sprintf(t_push,"T%d",label_num);
						push(t_push);
						label_num++;
						print_tok();
					}else if(opeEval(o_stack.data[o_stack.head-1],tok.charvalue) == 0){
						char temp[MAXIDLEN + 1];
						strcpy(temp,pop());
						o_pop();
						pop();
						push(temp);
						print_tok();
						break;
					}
					else{
						o_push(")");
						push(")");
						break;
					}
				}
			}else{
				while(true){
					if(opeEval(o_stack.data[o_stack.head-1],"$") == 1){
						char t3[MAXIDLEN+1];
						char t2[MAXIDLEN+1];
						char t1[MAXIDLEN+1];
						strcpy(t3,pop());
						strcpy(t2,pop());
						strcpy(t1,pop());
						o_pop();
						char intermediate_temp[50]; 
						sprintf(intermediate_temp,"%s,%s,%s,T%d",t2,t1,t3,label_num);
						i_push(intermediate_temp);
						char t_push[MAXIDLEN + 1];
						sprintf(t_push,"T%d",label_num);
						push(t_push);
						label_num++;
						print_tok();
					}
					else if(opeEval(o_stack.data[o_stack.head-1],"$") == 0){
						goto E;
					}
				}
			}
			getsym();
			print_tok();
		}

	}while(true);


	E:;
	if(i_stack.data == 0){

	}
	print_intermediate();
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
	int i;
	for(i=0;i<v_label;i++){
		fprintf(outfile,"label%d: data %d\n",v_table[i].label_count,v_table[i].value);
	}
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

void push(char* token){
	if(stack.head >= STACK_SIZE){
		error("stack over flow");
	}

	strcpy(stack.data[stack.head],token);
	(stack.head)++;

}

char* pop(void){
	char* ret;
	if(stack.head == 0){
		error("stack under flow");
	}
	(stack.head)--;
	ret = stack.data[stack.head];

	return ret;
}

void stack_initialize(void){
	stack.head = 0;	
}

void i_push(char* intermediate){
	if(i_stack.head >= 500){
		error("stack over flow");
	}

	strcpy(i_stack.data[i_stack.head],intermediate);
	i_stack.head++;

}

char* i_pop(void){
	char* ret;
	if(i_stack.head == 0){
		error("stack under flow");
	}
	(i_stack.head)--;
	ret = i_stack.data[i_stack.head];

	return ret;
}

void i_stack_initialize(void){
	i_stack.head = 0;	
}

void o_push(char* operand){
	if(o_stack.head >= 100){
		error("stack over flow");
	}

	strcpy(o_stack.data[o_stack.head],operand);
	o_stack.head++;

}

char* o_pop(void){
	char* ret;
	if(o_stack.head == 0){
		error("stack under flow");
	}
	(o_stack.head)--;
	ret = o_stack.data[o_stack.head];

	return ret;
}

void o_stack_initialize(void){
	o_stack.head = 0;	
}


// @param a preOpe
// @param b Ope
// @return 	f(a) > f(b) = 1
// 			f(a) = f(b) = 0
// 			f(a) < f(b) = -1
int opeEval(char* a,char* b){
	int f[8] = {2,2,4,4,6,0,11,0};
	int g[8] = {1,1,3,3,15,10,0,0};
	char ope[8][MAXIDLEN + 1] = {"+","-","*","div","!","(",")","$"};

	int f_eval = -1;
	int g_eval = -1;

	int i;
	for(i=0;i<8;i++){
		if(strcmp(a,ope[i]) == 0){
			f_eval = f[i];
		}
		if(strcmp(b,ope[i]) == 0){
			g_eval = g[i];
		}
	}

	if(f_eval == -1 || g_eval == -1){
		error("syntax error");
	}

	if(f_eval > g_eval){
		return 1;
	}else if(f_eval == g_eval){
		return 0;
	}else{
		return -1;
	}

}

// @return c = integer = true; 
bool checkDigit(char * c)
{

    int i;

    for (i=0; i<strlen(c); i++) {
        if (!(isdigit(c[i]))) {
            return false;
        } 
    }
    return true;

}