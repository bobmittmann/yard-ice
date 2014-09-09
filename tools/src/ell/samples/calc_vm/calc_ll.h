#ifndef __CALC_LL_H__
#define __CALC_LL_H__

#include <stdint.h>

/* Last symbol num:83  Num rules:46 */

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
#define T_ERR                   34
#define T_NOT                   35

#define NONTERM_BASE            36
#define N_PROGRAM               36
#define N_STAT                  37
#define N_COMPOUND              38
#define N_STAT_LIST             39
#define N_EXP                   40
#define N_CONDITION             41
#define N_SHIFT_EXP             42
#define N_RELATIONAL_EXP        43
#define N_ADDITIVE_EXP          44
#define N_SHIFT_EXP1            45
#define N_MULT_EXP              46
#define N_ADDITIVE_EXP1         47
#define N_UNARY_EXP             48
#define N_MULT_EXP1             49
#define N_PRIMARY_EXP           50

#define ACTION_BASE             51
#define A_OP_VAR_DECL           51
#define A_OP_LOOKUP_ID          52
#define A_OP_ASSIGN             53
#define A_OP_PRINT              54
#define A_OP_WHILE_BEGIN        55
#define A_OP_WHILE_COND         56
#define A_OP_WHILE_END          57
#define A_OP_EQU                58
#define A_OP_NEQ                59
#define A_OP_LT                 60
#define A_OP_GT                 61
#define A_OP_GTE                62
#define A_OP_LTE                63
#define A_OP_SHL                64
#define A_OP_ASR                65
#define A_OP_ADD                66
#define A_OP_SUB                67
#define A_OP_XOR                68
#define A_OP_OR                 69
#define A_OP_LOGIC_OR           70
#define A_OP_MUL                71
#define A_OP_DIV                72
#define A_OP_MOD                73
#define A_OP_AND                74
#define A_OP_LOGIC_AND          75
#define A_OP_INV                76
#define A_OP_MINUS              77
#define A_OP_NOT                78
#define A_OP_PUSH_INT           79
#define A_OP_PUSH_ID            80
#define A_OP_PUSH_TRUE          81
#define A_OP_PUSH_FALSE         82

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


