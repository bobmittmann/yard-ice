%token EOF DOT COMMA SEMICOLON COLON PLUS MINUS STAR SLASH PERCENT AMPERSAND BAR CARET TILDE EXCLAM QUEST EQUALS LESSTHEN GREATTHEN LBRACKET RBRACKET LPAREN RPAREN LBRACE RBRACE GTE LTE EQU NEQ ASR SHL LOGICOR LOGICAND BREAK CATCH CONTINUE ELSE FALSE FINALLY FOR FUNCTION IF RETURN THROW TRUE TRY VAR WHILE ID INT CHAR STRING ERR
%%

cmd_line		: stat_list_opt EOF
				;

stat_list_opt	:
				| stat_list
				;

stat_list		: stat stat_list_tail
				;

stat_list_tail	: 
				| ';' stat_list
				;

stat			: "var" 
				| "ls"
				| "db" db_cmd
				| "cfg" cfg_cmd
				| "str" str_cmd
				| "trig" trig_cmd
				| "alm" alm_cmd
				| "tbl" tbl_cmd
				| "grp" grp_cmd
				| "rx" FNAME
				| "cat" FNAME
				| "rm" FNAME
				| "run" FNAME
				| "rst" 
				| "set" set_var_opt
				| "sens" '[' exp ']' '.' dev_attr '=' exp
				| "mod" '[' exp ']' '.' dev_attr '=' exp
				;

trig_cmd		:
				| "sens" exp
				| "mod" exp
				;

tbl_cmd			:
				| exp
				;

alm_cmd			:
				| exp
				;

grp_cmd			:
				| exp
				;
				
db_cmd			:
				| "compile"
				| "stat"
				| "erase"
				;

cfg_cmd			:
				| "compile"
				| "erase"
				| "load"
				| "save"
				;

str_cmd			:
				| str_lst
				;

str_lst			: STRING str_lst_tail
				| ID str_lst_tail
				;

str_lst_tail	: 
				| str_lst
				;


set_var_opt 	:
				| dev_attr exp
				;

dev_attr		: "pw1"
				| "pw2"
				| "pw3"
				| "pw4"
				| "pw5"
				| "en"
				| "cfg"
				| "alm"
				| "tbl"
				;

exp				: additive_exp
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
				| '-' unary_exp { op_minus }
				;

primary_exp		: '(' exp ')' 
				| INT { op_push_int }
				| CHAR { op_push_int }
				| STRING { op_push_string }
				| "true" { op_push_true }
				| "false" { op_push_false }
				| VAR_ID { op_push_tmp } { op_attr }
				| FUN_ID { op_push_tmp } function_call { op_call_ret }
				| "sens" '[' exp ']' '.' dev_attr
				| "mod" '[' exp ']' '.' dev_attr
				;

