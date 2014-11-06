%token EOF DOT COMMA SEMICOLON COLON PLUS MINUS STAR SLASH PERCENT AMPERSAND BAR CARET TILDE EXCLAM QUEST EQUALS LESSTHEN GREATTHEN LBRACKET RBRACKET LPAREN RPAREN LBRACE RBRACE GTE LTE EQU NEQ ASR SHL LOGICOR LOGICAND BREAK CATCH CONTINUE ELSE FALSE FINALLY FOR FUNCTION IF RETURN THROW TRUE TRY VAR WHILE ID INT CHAR STRING ERR
%%

program 		: EOF
				| stat program
				| compound_stat program
				;

compound_stat	: '{' { op_blk_open } stat_list '}' { op_blk_close } 
				;

stat_list		:
				| stat stat_list
				;

stat			: "for" '(' exp_lst_opt ';' { op_for_init } exp { op_for_cond } ';' exp_lst_opt { op_for_after } ')' compound_stat { op_for_end }
				| "while" { op_while_begin } condition { op_while_cond } compound_stat { op_while_end }
				| "if" condition { op_if_cond } compound_stat else_opt { op_if_end }
				| "try" { op_try_begin } compound_stat catch_opt 
				| "throw" exp { op_throw } ';'
				| "var" var_list ';'
				| assign_or_call ';'
				;

else_opt		: 
				| "else" { op_if_else } compound_stat
				;

catch_opt		: { op_try_end }
				| "catch" { op_catch, op_blk_open } '(' ID { op_var_decl, op_push_tmp, op_assign } ')' '{' stat_list '}' { op_blk_close, op_catch_end }
				;

condition		: '(' exp ')'
				;

var_list 		: var var_list1
				;

var_list1 		: 
				| ',' var_list
				;

var				: ID { op_var_decl, op_push_tmp } var_assign_opt
				;

var_assign_opt  : { op_pop_tmp }
				| '=' exp { op_assign }
				;

assign_or_call  : ID { op_push_tmp } assign_or_call1
				;

assign_or_call1	: '=' exp { op_assign }
				| function_call { op_ret_discard }
				;

function_call	: { op_method } '(' arg_list_opt ')' { op_call } 
				;

arg_list_opt	: 
				| arg_list 
				;

arg_list		: exp { op_arg } arg_list1 
				;

arg_list1		: 
				| ',' arg_list 
				;

exp_lst_opt		: 
				| exp_lst
				;

exp_lst			: assign_or_call exp_lst1
				;
				
exp_lst1		: 
				| ',' exp_lst
				;

exp 			: and_exp or_exp
				;


or_exp			: 
				| '|' exp { op_or }
				| LOGICOR exp { op_logic_or }
				| '^' exp { op_xor }
				;

and_exp			: relational_exp and_exp1
				;

and_exp1		: 
				| '&' and_exp { op_and }
				| LOGICAND and_exp { op_logic_and }
				;

relational_exp	: shift_exp	relational_exp1
				;

relational_exp1	:
				| '<' relational_exp { op_lt }
				| '>' relational_exp { op_gt }
				| EQU relational_exp { op_equ }
				| NEQ relational_exp { op_neq }
				| GTE relational_exp { op_gte }
				| LTE relational_exp { op_lte }
				;

shift_exp		: additive_exp shift_exp1
				;

shift_exp1		: 
				| SHL shift_exp	{ op_shl }
				| ASR shift_exp	{ op_asr }
				;

additive_exp 	: mult_exp additive_exp1
				;

additive_exp1 	: 
				| '+' additive_exp { op_add }
				| '-' additive_exp { op_sub }
				;

mult_exp		: unary_exp mult_exp1		
				;

mult_exp1		: 
				| '/' mult_exp { op_div }
				| '%' mult_exp { op_mod }
				| '*' mult_exp { op_mul }
				;

unary_exp		: primary_exp
				| '~' unary_exp { op_inv }
				| '-' unary_exp { op_minus }
				| NOT unary_exp { op_not }
				;

primary_exp		: '(' exp ')'
				| INT { op_push_int }
				| CHAR { op_push_int }
				| STRING { op_push_string }
				| "true" { op_push_true }
				| "false" { op_push_false }
				| meth_or_attr 
				;

meth_or_attr	: ID { op_push_tmp } meth_or_attr1
				;

meth_or_attr1	: { op_attr }
				| function_call { op_call_ret }
				;

