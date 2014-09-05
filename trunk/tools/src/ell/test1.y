%token 	LT GT LTE GTE EQU NEQ SHL ASR PLUS MINUS BITOR XOR OR MUL DIV MOD BITAND AND LPAREN RPAREN INT FALSE TRUE NULL EOF
%%

prog	: exp  EOF 
		;

exp		: term exp1 
		;
exp1	: /*empty*/  
		| PLUS term exp1	
		| MINUS term exp1
		;
term  	: factor term1 ;

term1	:  /*empty*/ 
		| MUL factor term1  
		| DIV factor term1
		;	

factor	: LPAREN exp RPAREN
		| INT 
		| FALSE 
		| TRUE 
		| NULL 
		;

