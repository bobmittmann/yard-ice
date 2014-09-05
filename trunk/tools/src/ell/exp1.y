%token 	LT GT LTE GTE EQU NEQ SHL ASR PLUS MINUS BITOR XOR OR MUL DIV MOD BITAND AND LPAREN RPAREN INT FALSE TRUE NULL EOF
%%

prog			: exp  EOF 
				;

exp				: mult_exp exp1 
				;
exp1			: /*empty*/
				| PLUS exp	
				| MINUS exp
				;

mult_exp		: unary_exp mult_exp1
				;
mult_exp1		: /*empty*/
				| MUL mult_exp
				| DIV mult_exp
				| MOD mult_exp
				| BITAND mult_exp
				| AND mult_exp
				;

unary_exp		: factor
				| INV unary_exp
				| NOT unary_exp
				| PLUS unary_exp
				| MINUS unary_exp
				;

factor			: LPAREN exp RPAREN
				| INT 
				| FALSE 
				| TRUE 
				| NULL 
				;
