%token 	LT GT LTE GTE EQU NEQ SHL ASR PLUS MINUS BITOR XOR OR MUL DIV MOD BITAND AND LPAREN RPAREN INT FALSE TRUE NULL EOF ID DOT LBKT RBKT COMMA ASSIGN SEMICOLON LBRACE RBRACE FUNCTION VAR IF ELSE WHILE BREAK CONTINUE RETURN
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

exp_opt			: 
				| exp
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

exp 			: shift_exp exp1
				| LBKT arg_list_opt RBKT
				;

exp1 			: 
				| LT exp
				| GT exp
				| LTE exp
				| GTE exp
				| EQU exp
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
				| PLUS additive_exp 
				| MINUS additive_exp 
				| BITOR additive_exp 
				| XOR additive_exp 
				| OR additive_exp 
				;

mult_exp		: unary_exp mult_exp1		
				;

mult_exp1		: 
				| MUL mult_exp
				| DIV mult_exp
				| MOD mult_exp
				| BITAND mult_exp
				| AND mult_exp
				;

unary_exp		: primary_exp
				| INV unary_exp
				| NOT unary_exp
				| PLUS unary_exp
				| MINUS unary_exp
				;

primary_exp		: LPAREN exp RPAREN
				| INT
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

