#ifndef INC_PARSER_H
#define INC_PARSER_H

#ifdef USE_LEX
	extern char *yytext;
	int yylex();
	#define getToken  yylex
	#define lexeme  yytext
#else
	int getToken();
	extern char *lexeme;
#endif


extern int lookahead;

#define EOF  256
#define DOT  257
#define COMMA  258
#define SEMICOLON  259
#define COLON  260
#define PLUS  261
#define MINUS  262
#define STAR  263
#define SLASH  264
#define PERCENT  265
#define AMPERSAND  266
#define BAR  267
#define CARET  268
#define TILDE  269
#define EXCLAM  270
#define QUEST  271
#define EQUALS  272
#define LESSTHEN  273
#define GREATTHEN  274
#define LBRACKET  275
#define RBRACKET  276
#define LPAREN  277
#define RPAREN  278
#define LBRACE  279
#define RBRACE  280
#define GTE  281
#define LTE  282
#define EQU  283
#define NEQ  284
#define ASR  285
#define SHL  286
#define LOGICOR  287
#define LOGICAND  288
#define BREAK  289
#define CATCH  290
#define CONTINUE  291
#define ELSE  292
#define FALSE  293
#define FINALLY  294
#define FOR  295
#define FUNCTION  296
#define IF  297
#define RETURN  298
#define THROW  299
#define TRUE  300
#define TRY  301
#define VAR  302
#define WHILE  303
#define ID  304
#define INT_VAL  305
#define CHAR  306
#define STRING  307
#define ERR  308
#define BOOL  309
#define INT  310
#define FIX  311
#define INT_VAR  312
#define INT_FUN  313
#define FIX_VAR  314
#define FIX_FUN  315
#define BOOL_VAR  316
#define BOOL_FUN  317
#define NOT  318
#define FIX2INT  319
#define BOOL2INT  320

void program();
void stat();
void compound_stat();
void stat_list();
void for_lst_opt();
void int_exp();
void condition();
void else_opt();
void catch_opt();
void bool_var_list();
void int_var_list();
void fix_var_list();
void assign_or_call();
void function_call();
void fix_exp();
void bool_exp();
void arg_list_opt();
void arg_list();
void exp();
void arg_list1();
void for_lst();
void for_lst1();
void bool_var();
void bool_var_list1();
void bool_var_assign_opt();
void bool_and_exp();
void bool_or_exp();
void bool_unary_exp();
void bool_and_exp1();
void bool_pri_exp();
void rel_exp();
void int_rel_exp();
void int_var();
void int_var_list1();
void int_var_assign_opt();
void int_rel_exp1();
void and_exp();
void or_exp();
void shift_exp();
void and_exp1();
void int_add_exp();
void shift_exp1();
void int_mult_exp();
void int_add_exp1();
void int_unary_exp();
void int_mult_exp1();
void int_primary_exp();
void fix_var();
void fix_var_list1();
void fix_var_assign_opt();
void fix_rel_exp();
void fix_rel_exp1();
void fix_add_exp();
void fix_mult_exp();
void fix_add_exp1();
void fix_unary_exp();
void fix_mult_exp1();
void fix_primary_exp();

#endif
