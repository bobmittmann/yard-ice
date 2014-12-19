LL(1) Predictive Parser Table Generator and RDP Generator

Author: Ivo Mateljan 

This is simple recursive descent parser generator. It also can be used to build
LL(1) predictive parsing tables. I have made it for my students.

Version 0.2.			1 September 2003.


USAGE:
  ell  [-l]  [-f] [-t]  [file]
	  
OPTIONS:
		
-t :	generate output of machine-readable parsing tables.
		this disable generation of recursive descent parser code,
		which is a default action

-l :	generate listing of terminals, nonterminals and grammar 
		on the standard output.
	  
-s :	generate listing of `first', 'follow' and 'predict' sets
		as computed for the grammar.	
		
			  
file :	the name of the file containing the grammar.  If this
		argument is absent, the grammar is read from the
		standard input.
			
RESULT:				  
	If the grammar is free of errors and if the `-t' flag is not
	given, ll(1) recursive descent parser code is generated in a file named
	`file.c', where `file' is the name of the input file holding
	the grammar. 
	If the grammar was read from the standard input, the tables are written 
	to a file named: `llparse.c'.

	If the grammar is free of errors and if the `-t' flag is given, 
	ll(1) parsing tables are generated in a file named
	`file.tbl', where `file' is the name of the input file holding
	the grammar. If the grammar was read from the standard input, the 
	tables are written to a file named: `llparse.tbl'.
					
	If the grammar contains errors, diagnostic information is given.
	
Gramar file format is extremly simple:
----------------------------------------------------------------------
%token  list_of_token
%%
list_of_production_rules

Every production start in a new line and have the form:

LHS : RHS1               or  LHS : RHS1 | RHS2| ...| RHSn ;
	| RHS2
	|  ....
    ;

LHS is single nonterminal. It is defined with alternate RHSi. One RHSi can be empty. 
RHSi contain series of tokens and nonterminals which can be grouped, 
as regular expressions:
    
	()  - group                              i.e. sign : ('+' | '-' |  ) 
	()? - group is optional                  i.e. sign : ('+' | '-')? 
	()* - group repeats zero or more times,  i.e. expr : term ( ('+' | '-') term )*
	()+ - group repeats one or more times,   i.e. expr : (statement )+

Rule is terminated with semicolon.

Nonternimals and tokens are designates with : (alpha|_)(alpha|digit|_)*
Tokens can also be are designated with single quoted char (i..e '+', '\n' )

Coments are in C++ style  
	 multiline comments  -  can not be nested
	// to end of line comment

To-do-list:
- proper error recovery
- semantic actions
- ebnf implementation, but retain possibility of predictive parser table generation
*/

