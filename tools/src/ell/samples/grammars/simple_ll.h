#ifndef __SIMPLE_LL_H__
#define __SIMPLE_LL_H__

#include <stdint.h>

/* Last symbol num:44  Num rules:30 */

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
#define T_INT_VAR               21
#define T_EQUALS                22
#define T_FIX_VAR               23
#define T_STAR                  24
#define T_SLASH                 25
#define T_INT_NUM               26
#define T_FIX_NUM               27

#define NONTERM_BASE            28
#define N_PROGRAM               28
#define N_ASSIGN_EXP            29
#define N_INT_EXP               30
#define N_FIX_EXP               31
#define N_INT_MUL_EXP           32
#define N_INT_ADD_EXP           33
#define N_INT_UNY_EXP           34
#define N_INT_MUL_EXP1          35
#define N_INT_PRI_EXP           36
#define N_FIX_MUL_EXP           37
#define N_FIX_ADD_EXP           38
#define N_FIX_UNY_EXP           39
#define N_FIX_MUL_EXP1          40
#define N_FIX_PRI_EXP           41

#define ACTION_BASE             42
#define A_OP_FIX2INT            42
#define A_OP_INT2FIX            43

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

extern const const char * const simple_ll_sym[];

#ifdef __cplusplus
extern "C" {
#endif

int simple_ll_push(uint8_t * sp, unsigned int sym, unsigned int tok);
int simple_ll_start(uint8_t * sp);

#ifdef __cplusplus
}
#endif

#endif /* __SIMPLE_LL_H__ */


