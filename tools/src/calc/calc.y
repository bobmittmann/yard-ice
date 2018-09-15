%token EOF ASR SHL PLUS MINUS MUL DIV MOD OR AND XOR NOT INV ASSIGN COMMA SEMICOLON LPAREN RPAREN VAR INT ID ERR
%start program
%%

program 		: EOF
				| stat program
				;

stat			: SEMICOLON
				| VAR ID SEMICOLON { op_var_decl }
				| ID ASSIGN exp SEMICOLON { op_assign }
				| PRINT exp SEMICOLON { op_print }
				;

exp 			: additive_exp shift_exp
				;

shift_exp		: 
				| SHL exp { op_shl }
				| ASR exp { op_asr }
				;

additive_exp 	: mult_exp additive_exp1
				;

additive_exp1 	: 
				| PLUS additive_exp { op_add }
				| MINUS additive_exp { op_sub }
				| XOR additive_exp { op_xor }
				| OR additive_exp { op_or }
				;

mult_exp		: unary_exp mult_exp1		
				;

mult_exp1		: 
				| MUL mult_exp { op_mult }
				| DIV mult_exp { op_div }
				| MOD mult_exp { op_mod }
				| AND mult_exp { op_and }
				;

unary_exp		: primary_exp
				| INV unary_exp { op_inv }
				| NOT unary_exp { op_not }
				| MINUS unary_exp { op_minus }
				;

primary_exp		: LPAREN exp RPAREN
				| INT { op_push_int }
				| ID { op_push_id }
				;

