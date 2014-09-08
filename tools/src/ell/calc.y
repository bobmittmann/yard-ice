%token EOF ASR SHL LTE LT GTE GT EQ NEQ PLUS MINUS MUL DIV MOD OR BITOR AND BITAND XOR NOT INV ASSIGN DOT COMMA SEMICOLON COLON LBKT RBKT LPAREN RPAREN LBRACE RBRACE BREAK CONTINUE ELSE FALSE FOR FUNCTION IF NULL RETURN TRUE VAR WHILE INT ID STRING ERR
%%

program 		: EOF
				| element program
				;

element 		: FUNCTION ID LPAREN param_list_opt RPAREN compound_stat
				| stat
				;

param_list_opt	: 
				| param_list 
				;

param_list 		: ID param_list1 
				;

param_list1		: 
				| COMMA param_list 
				;

stat			: SEMICOLON
				| FOR LPAREN exp_lst_opt SEMICOLON exp SEMICOLON exp_lst_opt RPAREN compound_stat
				| IF condition compound_stat else_opt
				| WHILE condition compound_stat
				| BREAK SEMICOLON
				| CONTINUE SEMICOLON
				| RETURN exp_opt SEMICOLON
				| VAR var_list SEMICOLON
				| compound_stat
				| assign_or_call SEMICOLON
				;

else_opt		: 
				| ELSE compound_stat
				;

compound_stat	: LBRACE stat_list RBRACE
				;

stat_list		: 
				| stat stat_list
				;

condition		: LPAREN exp RPAREN
				;

var_list 		: var var_list1
				;

var_list1 		: 
				| COMMA var_list
				;

var				: ID var1
				;

var1			: 
				| ASSIGN exp
				;


assign_or_call  : ID assign_or_call1
				;

assign_or_call1	: ASSIGN exp
				| DOT assign_or_call
				| LBKT exp RBKT assign_or_call2
				| LPAREN arg_list_opt RPAREN assign_or_call3
				;

assign_or_call2	: ASSIGN exp
				| DOT assign_or_call
				;

assign_or_call3	:
				| DOT assign_or_call
				;

exp_lst_opt		: 
				| exp_lst
				;

exp_lst			: assign_or_call exp_lst1
				;
				
exp_lst1		: 
				| COMMA exp_lst
				;

exp_opt			: 
				| exp
				;

exp 			: shift_exp exp1
				| LBKT arg_list_opt RBKT
				;

exp1 			: 
				| LT exp
				| GT exp
				| LTE exp
				| GTE exp
				| EQ exp
				| NEQ exp
				;

shift_exp		: additive_exp shift_exp1
				;

shift_exp1		: 
				| SHL shift_exp
				| ASR shift_exp
				;

additive_exp 	: mult_exp additive_exp1
				;

additive_exp1 	: 
				| PLUS additive_exp { op_add }
				| MINUS additive_exp { op_sub }
				| BITOR additive_exp { op_or }
				| XOR additive_exp { op_xor }
				| OR additive_exp { op_or }
				;

mult_exp		: unary_exp mult_exp1		
				;

mult_exp1		: 
				| MUL mult_exp { op_mult }
				| DIV mult_exp { op_div }
				| MOD mult_exp { op_mod }
				| BITAND mult_exp { op_and }
				| AND mult_exp { op_and }
				;

unary_exp		: primary_exp
				| INV unary_exp { op_inv }
				| NOT unary_exp { op_not }
				| PLUS unary_exp
				| MINUS unary_exp { op_minus }
				;

primary_exp		: LPAREN exp RPAREN
				| INT { op_push_int }
				| FALSE 
				| TRUE 
				| NULL 
				| met_or_attr
				;

met_or_attr		: ID met_or_attr1
				;

met_or_attr1	: 
				| DOT met_or_attr
				| LBKT exp RBKT met_or_attr2
				| LPAREN arg_list_opt RPAREN met_or_attr2
				;

met_or_attr2	: 
				| DOT met_or_attr
				;

arg_list_opt	: 
				| arg_list 
				;

arg_list		: exp arg_list1 
				;

arg_list1		: 
				| COMMA arg_list 
				;

