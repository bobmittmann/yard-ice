prog			: exp  EOF 
				;

exp				: mult_exp exp1 
				;
exp1			: /*empty*/
				| PLUS exp {ADD}	
				| MINUS exp {SUB}
				;

mult_exp		: factor mult_exp1
				;
mult_exp1		: /*empty*/
				| MUL mult_exp
				| DIV mult_exp
				;

factor			: LPAREN exp RPAREN
				| INT 
				;

