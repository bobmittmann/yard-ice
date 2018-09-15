%token EOF ASR SHL PLUS MINUS MUL DIV MOD OR AND XOR INV ASSIGN COMMA SEMICOLON LPAREN RPAREN VAR INT ID ERR
%%

program 		: EOF
				| assign_exp
				;

assign_exp      : INT_VAR '=' int_exp
				| FIX_VAR '=' fix_exp
				;

int_exp			: int_mul_exp int_add_exp
				;

int_add_exp 	: 
				| '+' int_add_exp 
				| '-' int_add_exp 
				;

int_mul_exp		: int_uny_exp int_mul_exp1		
				;

int_mul_exp1	: 
				| '*' int_mul_exp 
				| '/' int_mul_exp 
				;

int_uny_exp		: int_pri_exp
				| '-' int_uny_exp 
				;

int_pri_exp		: INT_NUM 
				| FIX_NUM { op_fix2int }
				| '(' int_exp ')'
				;



fix_exp			: fix_mul_exp fix_add_exp
				;

fix_add_exp 	: 
				| '+' fix_add_exp 
				| '-' fix_add_exp 
				;

fix_mul_exp		: fix_uny_exp fix_mul_exp1		
				;

fix_mul_exp1	: 
				| '*' fix_mul_exp 
				| '/' fix_mul_exp 
				;

fix_uny_exp		: fix_pri_exp
				| '-' fix_uny_exp 
				;

fix_pri_exp		: FIX_NUM
				| INT_NUM { op_int2fix }
				| '(' fix_exp ')'
				;



