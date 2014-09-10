#ifndef __CALC_LL_H__
#define __CALC_LL_H__

#include <stdint.h>

/* Last symbol num:109  Num rules:68 */

#define T_EOF                    0
#define T_EQU                    1
#define T_NEQ                    2
#define T_LT                     3
#define T_GT                     4
#define T_GTE                    5
#define T_LTE                    6
#define T_LOR                    7
#define T_LAND                   8
#define T_ASR                    9
#define T_SHL                   10
#define T_PLUS                  11
#define T_MINUS                 12
#define T_MUL                   13
#define T_DIV                   14
#define T_MOD                   15
#define T_OR                    16
#define T_AND                   17
#define T_XOR                   18
#define T_INV                   19
#define T_ASSIGN                20
#define T_COMMA                 21
#define T_SEMICOLON             22
#define T_LPAREN                23
#define T_RPAREN                24
#define T_LBRACE                25
#define T_RBRACE                26
#define T_VAR                   27
#define T_PRINT                 28
#define T_WHILE                 29
#define T_INT                   30
#define T_ID                    31
#define T_TRUE                  32
#define T_FALSE                 33
#define T_IF                    34
#define T_ELSE                  35
#define T_FOR                   36
#define T_ERR                   37
#define T_NOT                   38

#define NONTERM_BASE            39
#define N_PROGRAM               39
#define N_STAT                  40
#define N_COMPOUND_STAT         41
#define N_STAT_LIST             42
#define N_EXP_LST_OPT           43
#define N_EXP                   44
#define N_CONDITION             45
#define N_ELSE_OPT              46
#define N_PRINT_LIST_OPT        47
#define N_VAR_LIST              48
#define N_ASSIGN_OR_CALL        49
#define N_PRINT_LIST            50
#define N_PRINT_LIST1           51
#define N_VAR                   52
#define N_VAR_LIST1             53
#define N_VAR_ASSIGN_OPT        54
#define N_ASSIGN_OR_CALL1       55
#define N_EXP_LST               56
#define N_EXP_LST1              57
#define N_SHIFT_EXP             58
#define N_RELATIONAL_EXP        59
#define N_ADDITIVE_EXP          60
#define N_SHIFT_EXP1            61
#define N_MULT_EXP              62
#define N_ADDITIVE_EXP1         63
#define N_UNARY_EXP             64
#define N_MULT_EXP1             65
#define N_PRIMARY_EXP           66

#define ACTION_BASE             67
#define A_OP_FOR_INIT           67
#define A_OP_FOR_COND           68
#define A_OP_FOR_AFTER          69
#define A_OP_FOR_END            70
#define A_OP_WHILE_BEGIN        71
#define A_OP_WHILE_COND         72
#define A_OP_WHILE_END          73
#define A_OP_IF_COND            74
#define A_OP_IF_END             75
#define A_OP_PRINT_END          76
#define A_OP_PRINT_INT          77
#define A_OP_PRINT_COMMA        78
#define A_OP_IF_ELSE            79
#define A_OP_VAR_DECL           80
#define A_OP_LOOKUP_ID          81
#define A_OP_PUSH_ID_ADDR       82
#define A_OP_ASSIGN             83
#define A_OP_EQU                84
#define A_OP_NEQ                85
#define A_OP_LT                 86
#define A_OP_GT                 87
#define A_OP_GTE                88
#define A_OP_LTE                89
#define A_OP_SHL                90
#define A_OP_ASR                91
#define A_OP_ADD                92
#define A_OP_SUB                93
#define A_OP_XOR                94
#define A_OP_OR                 95
#define A_OP_LOGIC_OR           96
#define A_OP_MUL                97
#define A_OP_DIV                98
#define A_OP_MOD                99
#define A_OP_AND               100
#define A_OP_LOGIC_AND         101
#define A_OP_INV               102
#define A_OP_MINUS             103
#define A_OP_NOT               104
#define A_OP_PUSH_INT          105
#define A_OP_LOAD_VAR          106
#define A_OP_PUSH_TRUE         107
#define A_OP_PUSH_FALSE        108

#define IS_A_TERMINAL(_X) ((_X) < NONTERM_BASE)
#ifdef ACTION_BASE
#define IS_A_NONTERMINAL(_X) ((_X) >= NONTERM_BASE && (_X) < ACTION_BASE)
#define IS_AN_ACTION(_X) ((_X) >= ACTION_BASE)
#define ACTION(_X) ((_X) - ACTION_BASE)
#else
#define IS_A_NONTERMINAL(_X) ((_X) >= NONTERM_BASE)
#define IS_AN_ACTION(_X) (0)
#define ACTION(_X) (0)
#endif

extern const const char * const ll_sym_tab[];

#ifdef __cplusplus
extern "C" {
#endif

int ll_rule_push(uint8_t * sp, unsigned int sym, unsigned int tok);
int ll_start(uint8_t * sp);

#ifdef __cplusplus
}
#endif

#endif /* __CALC_LL_H__ */


