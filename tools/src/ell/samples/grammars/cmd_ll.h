#ifndef __CMD_LL_H__
#define __CMD_LL_H__

#include <stdint.h>

/* Last symbol num:35  Num rules:25 */

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
#define T_EOL                   21
#define T_CMD                   22
#define T_NOT                   23

#define NONTERM_BASE            24
#define N_PROGRAM               24
#define N_STAT                  25
#define N_EXP                   26
#define N_CMD_ARG               27
#define N_MULT_EXP              28
#define N_ADDITIVE_EXP          29
#define N_UNARY_EXP             30
#define N_MULT_EXP1             31
#define N_PRIMARY_EXP           32
#define N_METH_OR_ATTR          33
#define N_METH_OR_ATTR1         34


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

extern const const char * const cmd_ll_sym[];

#ifdef __cplusplus
extern "C" {
#endif

int cmd_ll_push(uint8_t * sp, unsigned int sym, unsigned int tok);
int cmd_ll_start(uint8_t * sp);

#ifdef __cplusplus
}
#endif

#endif /* __CMD_LL_H__ */


