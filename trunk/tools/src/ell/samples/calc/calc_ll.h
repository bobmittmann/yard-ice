#ifndef __CALC_LL_H__
#define __CALC_LL_H__

#include <stdint.h>

/* Last symbol num:49  Num rules:28 */

#define T_EOF                    0
#define T_ASR                    1
#define T_SHL                    2
#define T_PLUS                   3
#define T_MINUS                  4
#define T_MUL                    5
#define T_DIV                    6
#define T_MOD                    7
#define T_OR                     8
#define T_AND                    9
#define T_XOR                   10
#define T_INV                   11
#define T_ASSIGN                12
#define T_COMMA                 13
#define T_SEMICOLON             14
#define T_LPAREN                15
#define T_RPAREN                16
#define T_VAR                   17
#define T_INT                   18
#define T_ID                    19
#define T_ERR                   20
#define T_PRINT                 21

#define NONTERM_BASE            22
#define N_PROGRAM               22
#define N_STAT                  23
#define N_EXP                   24
#define N_ADDITIVE_EXP          25
#define N_SHIFT_EXP1            26
#define N_MULT_EXP              27
#define N_ADDITIVE_EXP1         28
#define N_UNARY_EXP             29
#define N_MULT_EXP1             30
#define N_PRIMARY_EXP           31

#define ACTION_BASE             32
#define A_OP_VAR_DECL           32
#define A_OP_ASSIGN             33
#define A_OP_PRINT              34
#define A_OP_SHL                35
#define A_OP_ASR                36
#define A_OP_ADD                37
#define A_OP_SUB                38
#define A_OP_XOR                39
#define A_OP_OR                 40
#define A_OP_MUL                41
#define A_OP_DIV                42
#define A_OP_MOD                43
#define A_OP_AND                44
#define A_OP_INV                45
#define A_OP_MINUS              46
#define A_OP_PUSH_INT           47
#define A_OP_PUSH_ID            48

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


