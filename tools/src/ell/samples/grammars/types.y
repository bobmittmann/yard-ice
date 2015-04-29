%token EOF DOT COMMA SEMICOLON COLON PLUS MINUS STAR SLASH PERCENT AMPERSAND BAR CARET TILDE EXCLAM QUEST EQUALS LESSTHEN GREATTHEN LBRACKET RBRACKET LPAREN RPAREN LBRACE RBRACE GTE LTE EQU NEQ ASR SHL LOGICOR LOGICAND BREAK CATCH CONTINUE ELSE FALSE FINALLY FOR FUNCTION IF RETURN THROW TRUE TRY VAR WHILE ID INT_VAL CHAR STRING ERR
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

stat			: "for" '(' for_lst_opt ';' { op_for_init } int_exp { op_for_cond } ';' for_lst_opt { op_for_after } ')' compound_stat { op_for_end }
				| "while" { op_while_begin } condition { op_while_cond } compound_stat { op_while_end }
				| "if" condition { op_if_cond } compound_stat else_opt { op_if_end }
				| "try" { op_try_begin } compound_stat catch_opt 
				| "throw" int_exp { op_throw } ';'
				| "bool" bool_var_list ';'
				| "int" int_var_list ';'
				| "fix" fix_var_list ';'
				;

assign_or_call	: INT_VAR '=' int_exp { op_int_assign } ';'
				| INT_FUN function_call { op_ret_discard } ';'
				| FIX_VAR '=' fix_exp { op_fix_assign } ';'
				| FIX_FUN function_call { op_ret_discard } ';'
				| BOOL_VAR '=' bool_exp { op_bool_assign } ';'
				| BOOL_FUN function_call { op_ret_discard } ';'
				;

function_call	: { op_method } '(' arg_list_opt ')' { op_call } 
				;

else_opt		: 
				| "else" { op_if_else } compound_stat
				;

catch_opt		: { op_try_end }
				| "catch" { op_catch, op_blk_open } '(' ID { op_int_var_decl, op_push_tmp, op_assign } ')' '{' stat_list '}' { op_blk_close, op_catch_end }
				;

condition		: '(' int_exp ')'
				;


arg_list_opt	: 
				| arg_list 
				;

arg_list		: exp { op_arg } arg_list1 
				;

arg_list1		: 
				| ',' arg_list 
				;

exp				: int_exp 
//				| bool_exp 
				;

for_lst_opt		: 
				| for_lst
				;

for_lst			: assign_or_call for_lst1
				;
				
for_lst1		: 
				| ',' for_lst
				;

// ----------------------------------------------------------------------------
// Boolean
// ----------------------------------------------------------------------------


bool_var_list 	: bool_var bool_var_list1
				;

bool_var_list1 	: 
				| ',' bool_var_list
				;

bool_var		: ID { op_var_decl, op_push_tmp } bool_var_assign_opt
				;

bool_var_assign_opt  : { op_pop_tmp }
				| '=' bool_exp { op_bool_assign }
				;

bool_exp		: bool_and_exp bool_or_exp
				;

bool_or_exp		: 
				| LOGICOR bool_exp { op_logic_or }
				;

bool_and_exp	: bool_unary_exp bool_and_exp1
				;

bool_and_exp1	: 
				| LOGICAND bool_and_exp { op_logic_and }
				;

bool_unary_exp	: bool_pri_exp
				| NOT bool_unary_exp { op_not }
				;

bool_pri_exp    : "true" { op_push_true }
				| "false" { op_push_false }
				| '(' bool_exp ')'
				;

rel_exp			: int_rel_exp
				;

// ----------------------------------------------------------------------------
// Integers
// ----------------------------------------------------------------------------

int_var_list 	: int_var int_var_list1
				;

int_var_list1 	: 
				| ',' int_var_list
				;

int_var			: ID { op_var_decl, op_push_tmp } int_var_assign_opt
				;

int_var_assign_opt  : { op_pop_tmp }
				| '=' int_exp { op_int_assign }
				;


int_rel_exp		: int_exp int_rel_exp1
				;

int_rel_exp1	: '<' int_exp { op_lt }
				| '>' int_exp { op_gt }
				| EQU int_exp { op_equ }
				| NEQ int_exp { op_neq }
				| GTE int_exp { op_gte }
				| LTE int_exp { op_lte }
				;

int_exp			: and_exp or_exp
				;

or_exp			: 
				| '|' int_exp { op_or }
				| '^' int_exp { op_xor }
				;

and_exp			: shift_exp and_exp1
				;

and_exp1		: 
				| '&' and_exp { op_and }
				;

shift_exp		: int_add_exp shift_exp1
				;

shift_exp1		: 
				| SHL shift_exp	{ op_shl }
				| ASR shift_exp	{ op_asr }
				;

int_add_exp 	: int_mult_exp int_add_exp1
				;

int_add_exp1 	: 
				| '+' int_add_exp { op_add }
				| '-' int_add_exp { op_sub }
				;

int_mult_exp	: int_unary_exp int_mult_exp1		
				;

int_mult_exp1	: 
				| '/' int_mult_exp { op_div }
				| '%' int_mult_exp { op_mod }
				| '*' int_mult_exp { op_mul }
				;

int_unary_exp	: int_primary_exp
				| '~' int_primary_exp { op_inv }
//				| '-' int_primary_exp { op_minus }
				;

int_primary_exp	: INT_VAL { op_push_int }
				| CHAR { op_push_int }
				| INT_VAR
				| INT_FUN function_call { op_call_int_ret } 
				| FIX2INT '(' fix_exp ')'
				| BOOL2INT '(' fix_exp ')'
				| '(' int_exp ')'
				;
// ----------------------------------------------------------------------------
// Fixed point 
// ----------------------------------------------------------------------------

fix_var_list 	: fix_var fix_var_list1
				;

fix_var_list1 	: 
				| ',' fix_var_list
				;

fix_var			: ID { op_var_decl, op_push_tmp } fix_var_assign_opt
				;

fix_var_assign_opt  : { op_pop_tmp }
				| '=' fix_exp { op_fix_assign }
				;

fix_rel_exp		: fix_exp fix_rel_exp1
				;

fix_rel_exp1	: '<' fix_exp { op_lt }
				| '>' fix_exp { op_gt }
				| EQU fix_exp { op_equ }
				| NEQ fix_exp { op_neq }
				| GTE fix_exp { op_gte }
				| LTE fix_exp { op_lte }
				;

fix_exp 		: fix_add_exp
				;

fix_add_exp		: fix_mult_exp fix_add_exp1
				;

fix_add_exp1 	: 
				| '+' fix_add_exp { op_add_fix }
				| '-' fix_add_exp { op_sub_fix }
				;

fix_mult_exp	: fix_unary_exp fix_mult_exp1		
				;

fix_mult_exp1	: 
				| '/' fix_mult_exp { op_div_fix }
				| '*' fix_mult_exp { op_mul_fix }
				;

fix_unary_exp	: fix_primary_exp
				| '-' fix_primary_exp { op_minus_fix }
				;

fix_primary_exp	: FIX { op_push_fix }
				| FIX_VAR
//				| FIX_FUN function_call { op_call_fix_ret } 
//				| INT2FIX '(' int_exp ')'
//				| '(' fix_exp ')'
				;

