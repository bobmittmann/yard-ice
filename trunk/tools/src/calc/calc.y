program 		: 
				| element program
				;

element 		: 'function' 'id' '(' param_list_opt ')' compound_stat
				;

stat			: ';'
				| 'if' condition compound_stat
				| 'if' condition compound_stat 'else' compound_stat
				| 'while' condition compound_stat
				| 'break' ';'
				| 'continue' ';'
				| 'return' expression_opt ';'
				| 'var' var_list ';'
				| compound_stat
				| assign_or_call ';'
				;

compound_stat	: '{' stat_list '}'
				;

stat_list		: 
				| stat stat_list
				;

param_list_opt	: 
				| param_list 
				;

param_list 		: 'id' 
				| 'id' ',' param_list
				;

condition		: '(' exp ')'
				;

var_list 		: var
				| var ',' var_list
				;

var				: 'id'
				| 'id' '=' exp
				;

expression_opt	: 
				| exp
				;

assign_or_call  : 'id' '=' exp
				| 'id' '.' assign_or_call 
				| 'id' '[' exp ']' '=' exp
				| 'id' '[' exp ']' '.' assign_or_call
				| 'id' '(' arg_list_opt ')'
				| 'id' '(' arg_list_opt ')' '.' assign_or_call
				;

arg_list_opt	: 
				| arg_list
				;

arg_list		: exp  
				| exp ',' arg_list
				;

exp 			: '[' arg_list_opt ']'
				| shift_exp
				| shift_exp '<' exp
				| shift_exp '>' exp
				| shift_exp '<=' exp
				| shift_exp '>=' exp
				| shift_exp '==' exp
				| shift_exp '!=' exp
				;

shift_exp		: additive_exp
				| additive_exp '<<' shift_exp
				| additive_exp '>>' shift_exp
				;

additive_exp 	: mult_exp
				| mult_exp '+' additive_exp 
				| mult_exp '-' additive_exp 
				| mult_exp '|' additive_exp 
				| mult_exp '^' additive_exp 
				| mult_exp '||' additive_exp 
				;

mult_exp		: unary_exp
				| unary_exp '*' mult_exp
				| unary_exp '/' mult_exp
				| unary_exp '%' mult_exp
				| unary_exp '&' mult_exp
				| unary_exp '&&' mult_exp
				;

unary_exp		: primary_exp
				| '~' unary_exp
				| '!' unary_exp
				| '+' unary_exp
				| '-' unary_exp
				;

primary_exp		: '(' exp ')'
				| 'int' 
				| 'float'
				| 'string'
				| 'false' 
				| 'true' 
				| 'null' 
				| method_or_attr
				;

method_or_attr	: 'id' 
				| 'id' '.' method_or_attr
				| 'id' '[' exp ']'
				| 'id' '[' exp ']' '.' method_or_attr
				| 'id' '(' arg_list_opt ')'
				| 'id' '(' arg_list_opt ')' '.' method_or_attr
				;

