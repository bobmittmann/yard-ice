program 		: <empty>
				| element program

element 		: 'function' id '(' param_list_opt ')' compound_stat
				| stat

param_list_opt	: <empty>
				| param_list 

param_list 		: <id> 
				| <id> ',' param_list

compound_stat	: '{' stat_list '}'

stat_list		: <empty>
				| stat stat_list

stat			: ';'
				| 'if' condition stat 
				| 'if' condition stat 'else' stat 
				| 'while' condition stat 
				| 'break' ';'
				| 'continue' ';'
				| 'return' expression_opt ';'
				| compound_stat
				| var_list_or_exp ';'

condition		: '(' exp ')'

var_list_or_exp	: 'var' var_list
				| exp

var_list 		: var
				| var ',' var_list

var				: <id>
				| <id> '=' assignment_exp

expression_opt	: <empty>
				| exp

exp 			: assignment_exp
				| assignment_exp ',' exp 

assignment_exp	: logical_or_exp
				| member_exp '=' assignment_exp

logical_or_exp	: logical_and_exp
				| logical_and_exp '||' logical_or_exp

logical_and_exp : bitwise_or_exp
				| bitwise_or_exp '&&' logical_and_exp 

bitwise_or_exp	: bitwise_xor_exp
				| bitwise_xor_exp '|' bitwise_or_exp

bitwise_xor_exp : bitwise_and_exp
				| bitwise_and_exp '^' bitwise_xor_exp

bitwise_and_exp : equality_exp
				| equality_exp '&' bitwise_and_exp

equality_exp	: relational_exp
				| relational_exp '==' equality_exp 
				| relational_exp '!=' equality_exp 

relational_exp	: shift_exp
				| shift_exp '<' relational_exp
				| shift_exp '>' relational_exp
				| shift_exp '<=' relational_exp
				| shift_exp '>=' relational_exp

shift_exp		: additive_exp
				| additive_exp '<<' shift_exp
				| additive_exp '>>' shift_exp

additive_exp 	: mult_exp
				| mult_exp '+' additive_exp 
				| mult_exp '-' additive_exp 

mult_exp		: unary_exp
				| unary_exp '*' mult_exp
				| unary_exp '/' mult_exp
				| unary_exp '%' mult_exp

unary_exp		: member_exp
				| '~' unary_exp
				| '!' unary_exp
				| '+' unary_exp
				| '-' unary_exp

member_exp		: primary_exp
				| primary_exp '.' member_exp
				| primary_exp '[' exp ']'
				| primary_exp '(' arg_list_opt ')'

arg_list_opt	: empty
				| arg_list

arg_list		: assignment_exp
				| assignment_exp ',' arg_list

primary_exp		: '(' exp ')'
				| <id> 
				| <int_literal> 
				| <float_literal> 
				| <string_literal> 
				| <false> 
				| <true> 
				| <null> 


