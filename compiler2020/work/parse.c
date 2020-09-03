#include "parse.h"
#include "rc.c"

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
		bool offset_flag = false;
		char procedure_name[MAXIDLEN + 1];
		
		if(s_table_count != 0){
			for(i=0;i<s_table_procedure[s_table_count].head;i++){
				if(strcmp(tok.charvalue,s_table_procedure[s_table_count].table[i].v) == 0){
					var_address = s_table_procedure[s_table_count].table[i].addr;
					offset_flag = true;
					break;
				}
			}
		}

		if(offset_flag == false){
			for(i=0;i<S_TABLELEN;i++){
				if(strcmp(tok.charvalue,s_table[i].v) == 0){
					var_address = s_table[i].addr;
					break;
				}
			}
		}

		for(i=0;i<p_table.head;i++){
			if(strcmp(tok.charvalue,p_table.v[i]) == 0){
				var_address = i;
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
			int n = getRegister();
			expression(n);
			if(offset_flag == false){
				fprintf(outfile,"store r%d,%d\n",n,var_address);
			}else{
				fprintf(outfile,"store r%d,%d(BR)\n",n,var_address);
			}
			freeRegister(n);
		}else{
		
			int arg_count = 0;
			// inline paramlist 
			if(tok.value == LPAREN){
				print_tok();
			}else{
				error("not found pramlist");
			}
			do{
				getsym();
				int register_index = getRegister();
				expression(register_index);
				fprintf(outfile,"push r%d\n",register_index);
				arg_count++;
				freeRegister(register_index);
			}while(tok.value == COMMA);
			if(tok.value == RPAREN){
				getsym();
				print_tok();
			}
			
			fprintf(outfile,"call P%d\n",var_address);
			fprintf(outfile,"subi SP,%d\n",arg_count);
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
			int target_label = jmp_label;
			int target_endlabel;
			jmp_label++;
			if(tok.value == THEN){
				getsym();
				statement();
				if(tok.value == ELSE){
					target_endlabel = jmp_label;
					jmp_label++;
					fprintf(outfile,"jmp L%d\n",target_endlabel);
				}
				fprintf(outfile,"L%d:\n",target_label);
				if(tok.value == ELSE){
					getsym();
					statement();
					fprintf(outfile,"L%d:\n",target_endlabel);
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
				bool offset_flag = false;

				if(s_table_count != 0){
					for(i=0;i<s_table_procedure[s_table_count].head;i++){
						if(strcmp(tok.charvalue,s_table_procedure[s_table_count].table[i].v) == 0){
							int n = getRegister();
							fprintf(outfile,"load r%d,%d(BR)\n",n,s_table_procedure[s_table_count].table[i].addr);
							fprintf(outfile,"writed r%d\n",n);
							freeRegister(n);
							flag = 1;
							break;
						}
					}
				}
				if(offset_flag == false){
					for(i=0;i<S_TABLELEN;i++){
						if(strcmp(tok.charvalue,s_table[i].v) == 0){
							int n = getRegister();
							fprintf(outfile,"load r%d,%d\n",n,s_table[i].addr);
							fprintf(outfile,"writed r%d\n",n);
							freeRegister(n);
							flag = 1;
							break;
						}
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
					int n = getRegister();
					fprintf(outfile,"loadi r%d,' '\n",n);
					fprintf(outfile,"writec r%d\n",n);
					freeRegister(n);
					getsym();
					print_tok();
				}
			}
			int n = getRegister();
			fprintf(outfile,"loadi r%d,'\\n'\n",n);
			fprintf(outfile,"writec r%d\n",n);
			freeRegister(n);
		}
	}else{
		error("syntax error in statement");
	}
}

// rnに計算結果を返す
void expression(int target){
	print_tok();
	i_stack_initialize();
	o_stack_initialize();
	int paren_flag = 0;
	
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
						sprintf(intermediate_temp,"%s\t%s\t%s\tT%d",t2,t1,t3,label_num);
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
						sprintf(intermediate_temp,"%s\t%s\t%s\tT%d",t2,t1,t3,label_num);
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
						sprintf(intermediate_temp,"%s\t%s\t%s\tT%d",t2,t1,t3,label_num);
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
						sprintf(intermediate_temp,"%s\t%s\t%s\tT%d",t2,t1,t3,label_num);
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
						sprintf(intermediate_temp,"%s\t%s\t%s\tT%d",t2,t1,t3,label_num);
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
				paren_flag++;
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
						sprintf(intermediate_temp,"%s\t%s\t%s\tT%d",t2,t1,t3,label_num);
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
			}else if(tok.value == RPAREN && paren_flag > 0){
				paren_flag--;
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
						sprintf(intermediate_temp,"%s\t%s\t%s\tT%d",t2,t1,t3,label_num);
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
						sprintf(intermediate_temp,"%s\t%s\t%s\tT%d",t2,t1,t3,label_num);
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


	E:

	print_intermediate();

	if(i_stack.head == 0){
		char value[MAXIDLEN + 1];
		strcpy(value,pop());
		if(checkDigit(value)){
			int value_int = atoi(value);
			if(-32768<=value_int && value_int <=32767){
				fprintf(outfile,"loadi r%d,%s\n",target,value);
			}else{
				fprintf(outfile,"load r%d,label%d\n",target,v_label);
				v_table[v_label].label_count = v_label;
				v_table[v_label].value = value_int;
				v_label++;
			}
		}else{
			int i;
			int var_address = -1;
			bool offset_flag = false;

			if(s_table_procedure[s_table_count].head != 0){
				for(i=0;i<s_table_procedure[s_table_count].head;i++){
					if(strcmp(value,s_table_procedure[s_table_count].table[i].v) == 0){
						var_address = s_table_procedure[s_table_count].table[i].addr;
						offset_flag = true;
						break;
					}
				}
			}
			if(offset_flag == false){
				for(i=0;i<S_TABLELEN;i++){
					if(strcmp(value,s_table[i].v) == 0){
						var_address = s_table[i].addr;
						break;
					}
				}
			}
			if(offset_flag == true){
				fprintf(outfile,"load r%d,%d(BR)\n",target,var_address);
			}else{
				fprintf(outfile,"load r%d,%d\n",target,var_address);
			}
		}
	}else{
		int T[100]={0};
		int i;
		for(i=0;i<i_stack.head;i++){
			char s1[10];
			char s2[MAXIDLEN+1];
			char s3[MAXIDLEN+1];
			char s4[MAXIDLEN+1];
			int index1 = -1;
			int index2 = -1;
			int index3 = -1;
			int index4 = -1;
			
			sscanf(i_stack.data[i],"%s\t%s\t%s\t%s",s1,s2,s3,s4);
			// printf("s2=%s",s2);
			sscanf(s1,"T%d",&index1);
			sscanf(s2,"T%d",&index2);
			sscanf(s3,"T%d",&index3);
			sscanf(s4,"T%d",&index4);
			
			if(index1 != -1){
				T[index1]++;
			}
			if(index2 != -1){
				T[index2]++;
			}
			if(index3 != -1){
				T[index3]++;
			}
			if(index4 != -1){
				T[index4]++;
			}
		}


		
		char register_mem[4][MAXIDLEN + 1] = {"\0","\0","\0","\0"};
		for(i=0;i<i_stack.head;i++){

			char ope[10];
			char src1[MAXIDLEN+1];
			char src2[MAXIDLEN+1];
			char dst[MAXIDLEN+1];
			int register1 = -1;
			int register2 = -1;

			sscanf(i_stack.data[i],"%s\t%s\t%s\t%s",ope,src1,src2,dst);
			printf("ope=%s\tsrc1=%s\tsrc2=%s\tdst=%s\n",ope,src1,src2,dst);

			int index1 = -1;
			int index2 = -1;
			int index3 = -1;

			sscanf(src1,"T%d",&index1);
			sscanf(src2,"T%d",&index2);
			sscanf(dst,"T%d",&index3);
			if(index1 != -1){
				T[index1]--;
			}
			if(index2 != -1){
				T[index2]--;
			}
			if(index3 != -1){
				T[index3]--;
			}

			// register cache scan
			int j;
			for(j=0;j<4;j++){
				if(strcmp(src1,register_mem[j]) == 0){
					register1 = j;
					rewordResiter(j);
				}
				if(strcmp(src2,register_mem[j]) == 0){
					register2 = j;
					rewordResiter(j);
				}
			}

			if(register1 == -1){
				if(checkDigit(src1) == true){
					int src1_int = atoi(src1);
					if(-32768 <= src1_int && src1_int <= 32767){
						register1 = getRegister();
						fprintf(outfile,"loadi r%d,%s\n",register1,src1);
						strcpy(register_mem[register1],src1);
					}else{
						register1 = getRegister();
						fprintf(outfile,"load r%d,label%d\n",register1,v_label);
						v_table[v_label].label_count = v_label;
						v_table[v_label].value = src1_int;
						v_label++;
						strcpy(register_mem[register1],src1);
					}
				}else{
					// identifier read
					int i_read;
					int var_address = -1;
					bool offset_flag = false;

					if(s_table_procedure[s_table_count].head != 0){
						for(i_read=0;i_read<s_table_procedure[s_table_count].head;i_read++){
							if(strcmp(src1,s_table_procedure[s_table_count].table[i_read].v) == 0){
								var_address = s_table_procedure[s_table_count].table[i_read].addr;
								offset_flag = true;
								break;
							}
						}
					}
					if(offset_flag == false){
						for(i_read=0;i_read<S_TABLELEN;i_read++){
							if(strcmp(src1,s_table[i_read].v) == 0){
								var_address = s_table[i_read].addr;
								break;
							}
						}
					}

					register1 = getRegister();
					if(offset_flag == true){
						fprintf(outfile,"load r%d,%d(BR)\n",register1,var_address);
					}else{
						fprintf(outfile,"load r%d,%d\n",register1,var_address);
					}
					strcpy(register_mem[register1],src1);
				}
			}

			if(register2 == -1){
				if(checkDigit(src2) == true){
					int src2_int = atoi(src2);
					if(-32768 <= src2_int && src2_int <= 32767){
						register2 = getRegister();
						fprintf(outfile,"loadi r%d,%s\n",register2,src2);
						strcpy(register_mem[register2],src2);
					}else{
						register2 = getRegister();
						fprintf(outfile,"load r%d,label%d\n",register2,v_label);
						v_table[v_label].label_count = v_label;
						v_table[v_label].value = src2_int;
						v_label++;
						strcpy(register_mem[register1],src2);
					}
				}else{
					// identifier read
					int i_read;
					int var_address = -1;
					bool offset_flag = false;

					if(s_table_procedure[s_table_count].head != 0){
						for(i_read=0;i_read<s_table_procedure[s_table_count].head;i_read++){
							if(strcmp(src2,s_table_procedure[s_table_count].table[i_read].v) == 0){
								var_address = s_table_procedure[s_table_count].table[i_read].addr;
								offset_flag = true;
								break;
							}
						}
					}
					if(offset_flag == false){
						for(i_read=0;i_read<S_TABLELEN;i_read++){
							if(strcmp(src2,s_table[i_read].v) == 0){
								var_address = s_table[i_read].addr;
								break;
							}
						}
					}

					register2 = getRegister();
					if(offset_flag == true){
						fprintf(outfile,"load r%d,%d(BR)\n",register2,var_address);
					}else{
						fprintf(outfile,"load r%d,%d\n",register2,var_address);
					}
					strcpy(register_mem[register2],src2);
				}
			}
			
			if(strcmp(ope,"+") == 0){
				fprintf(outfile,"addr r%d,r%d\n",register1,register2);
				strcpy(register_mem[register1],dst);
			}else if(strcmp(ope,"-") == 0){
				fprintf(outfile,"subr r%d,r%d\n",register1,register2);
				strcpy(register_mem[register1],dst);
			}else if(strcmp(ope,"*") == 0){
				fprintf(outfile,"mulr r%d,r%d\n",register1,register2);
				strcpy(register_mem[register1],dst);
			}else if(strcmp(ope,"div") == 0){
				fprintf(outfile,"divr r%d,r%d\n",register1,register2);
				strcpy(register_mem[register1],dst);
			}

			if(i == i_stack.head-1){
				if(register1 != target){
					fprintf(outfile,"loadr r%d,r%d\n",target,register1);
				}
			}

			freeRegister(register1);
			freeRegister(register2);
			int i_register = 0;
			for(i_register = 0;i_register<4;i_register++){
				int num = -1;
				sscanf(register_mem[i_register],"T%d",&num);
				if(num != -1){
					if(T[num] != 0){
						rewordResiter(i_register);
					}
				}
			}
	
		}
		
	}
	
}

void outblock(void){
	p_table.head = 0;

	int address = 0;
	int address_diff = 0;
	print_tok();
	if(tok.attr == RWORD && tok.value == VAR){
		do{
			address_diff++;
			getsym();
			print_tok();
			strcpy(s_table[address].v,tok.charvalue);
			s_table[address].addr = address;
			address++;
			getsym();
			print_tok();
		}while(tok.value == COMMA);
		fprintf(outfile,"addi SP,%d\n",address_diff);
		fprintf(outfile,"jmp MAIN\n");

		if(tok.value == SEMICOLON){
			getsym();
		}

		while(true){
			if(tok.attr == RWORD && tok.value == PROCEDURE){
				s_table_count++;
				print_tok();
				getsym();
				if(tok.attr == IDENTIFIER){
					strcpy(p_table.v[p_table.head],tok.charvalue);
					fprintf(outfile,"P%d:\n",p_table.head);
					p_table.head++;
					fprintf(outfile,"push BR\n");
					fprintf(outfile,"loadr BR,SP\n");
					getsym();
					inblock();
				}else{
					error("not found ident");
				}
				if(tok.value == SEMICOLON){
					getsym();
				}

			}else{
				s_table_count--;
				break;
			}
		}
		fprintf(outfile,"MAIN:\n");
		statement();
	
	}
	print_symboltable();
	fprintf(outfile,"halt\n");
	int i;
	for(i=0;i<v_label;i++){
		fprintf(outfile,"label%d: data %d\n",v_table[i].label_count,v_table[i].value);
	}
}

void inblock(void){
	s_table_procedure[s_table_count].head = 0;
	int var_count_arg = 0;
	print_tok();
	if(tok.value == LPAREN){
		do{
			getsym();
			print_tok();
			strcpy(s_table_procedure[s_table_count].table[s_table_procedure[s_table_count].head].v,tok.charvalue);
			s_table_procedure[s_table_count].head++;
			var_count_arg++;
			getsym();
			print_tok();

		}while(tok.value == COMMA);
		int i;
		for(i=0;i<s_table_procedure[s_table_count].head;i++){
			s_table_procedure[s_table_count].table[i].addr = -var_count_arg + i - 1;
		}
		if(tok.value == RPAREN){
			getsym();
		}else{
			error("not found )");
		}
	}else{
		error("not found (");
	}
	if(tok.value == SEMICOLON){
		getsym();
		print_tok();
	}else{
		error("not found ;");
	}

	int var_count_local = 0;

	if(tok.attr == RWORD && tok.value == VAR){
		int offset = 1;
		do{
			getsym();
			strcpy(s_table_procedure[s_table_count].table[s_table_procedure[s_table_count].head].v,tok.charvalue);
			s_table_procedure[s_table_count].table[s_table_procedure[s_table_count].head].addr = offset;
			offset++;
			s_table_procedure[s_table_count].head++;
			var_count_local++;
			getsym();
			print_tok();
		}while(tok.value == COMMA);
		if(tok.value == SEMICOLON){
			print_tok();
			getsym();
		}
		fprintf(outfile,"addi SP,%d\n",var_count_local);
	}

	statement();
	fprintf(outfile,"subi SP,%d\n",var_count_local);
	fprintf(outfile,"pop BR\n");
	fprintf(outfile,"return\n");


}

void condition(int index){
	// fprintf(outfile,"push r0\n");
	int register1 = -1;
	int register2 = -1;
	register1 = getRegister();
	expression(register1);
	// fprintf(outfile,"push r1\n");
	// fprintf(outfile,"loadr r1,r0\n");
	print_tok();
	if(tok.attr == SYMBOL){
		if(tok.value == EQL){
			getsym();
			// print_tok();
			register2 = getRegister();
			expression(register2);
			fprintf(outfile,"cmpr r%d,r%d\n",register1,register2);
			// fprintf(outfile,"pop r1\n");
			// fprintf(outfile,"pop r0\n");
			fprintf(outfile,"jnz L%d\n",index);
		}else if(tok.value == NOTEQL){
			getsym();
			// print_tok();
			register2 = getRegister();
			expression(register2);
			fprintf(outfile,"cmpr r%d,r%d\n",register1,register2);
			// fprintf(outfile,"pop r1\n");
			// fprintf(outfile,"pop r0\n");
			fprintf(outfile,"jz L%d\n",index);
		}else if(tok.value == LESSTHAN){
			getsym();
			// print_tok();
			register2 = getRegister();
			expression(register2);
			fprintf(outfile,"cmpr r%d,r%d\n",register1,register2);
			// fprintf(outfile,"pop r1\n");
			// fprintf(outfile,"pop r0\n");
			fprintf(outfile,"jge L%d\n",index);
		}else if(tok.value == GRTRTHAN){
			getsym();
			// print_tok();
			register2 = getRegister();
			expression(register2);
			fprintf(outfile,"cmpr r%d,r%d\n",register1,register2);
			// fprintf(outfile,"pop r1\n");
			// fprintf(outfile,"pop r0\n");
			fprintf(outfile,"jle L%d\n",index);
		}else if(tok.value == LESSEQL){
			getsym();
			// print_tok();
			register2 = getRegister();
			expression(register2);
			fprintf(outfile,"cmpr r%d,r%d\n",register1,register2);
			// fprintf(outfile,"pop r1\n");
			// fprintf(outfile,"pop r0\n");
			fprintf(outfile,"jgt L%d\n",index);
		}else if(tok.value == GRTREQL){
			getsym();
			// print_tok();
			register2 = getRegister();
			expression(register2);
			fprintf(outfile,"cmpr r%d,r%d\n",register1,register2);
			// fprintf(outfile,"pop r1\n");
			// fprintf(outfile,"pop r0\n");
			fprintf(outfile,"jlt L%d\n",index);
		}else{
			error("syntax error in condition");
		}
	}else{
		error("syntax error in condition");
	}

	freeRegister(register1);
	freeRegister(register2);

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
// 			f(a) == f(b) = 0
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

// @return c == integer = true; 
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