#ifndef __MICROJS_LL_H__
#define __MICROJS_LL_H__

#include <stdint.h>

/* Last symbol num:127  Num rules:76 */

#define T_EOF                    0
#define T_DOT                    1
#define T_COMMA                  2
#define T_SEMICOLON              3
#define T_COLON                  4
#define T_LBRACKET               5
#define T_RBRACKET               6
#define T_LPAREN                 7
#define T_RPAREN                 8
#define T_LBRACE                 9
#define T_RBRACE                10
#define T_ASR                   11
#define T_SHL                   12
#define T_LTE                   13
#define T_LT                    14
#define T_GTE                   15
#define T_GT                    16
#define T_EQU                   17
#define T_NEQ                   18
#define T_PLUS                  19
#define T_MINUS                 20
#define T_MUL                   21
#define T_DIV                   22
#define T_MOD                   23
#define T_OR                    24
#define T_LOR                   25
#define T_AND                   26
#define T_LAND                  27
#define T_XOR                   28
#define T_NOT                   29
#define T_INV                   30
#define T_QUEST                 31
#define T_ASSIGN                32
#define T_ELSE                  33
#define T_FALSE                 34
#define T_FOR                   35
#define T_FUNCTION              36
#define T_IF                    37
#define T_TRUE                  38
#define T_VAR                   39
#define T_WHILE                 40
#define T_CONTINUE              41
#define T_BREAK                 42
#define T_RETURN                43
#define T_ID                    44
#define T_INT                   45
#define T_STRING                46
#define T_ERR                   47

#define NONTERM_BASE            48
#define N_PROGRAM               48
#define N_STAT                  49
#define N_COMPOUND_STAT         50
#define N_STAT_LIST             51
#define N_EXP_LST_OPT           52
#define N_EXP                   53
#define N_CONDITION             54
#define N_ELSE_OPT              55
#define N_VAR_LIST              56
#define N_ASSIGN_OR_CALL        57
#define N_VAR                   58
#define N_VAR_LIST1             59
#define N_VAR_ASSIGN_OPT        60
#define N_ASSIGN_OR_CALL1       61
#define N_ARG_LIST_OPT          62
#define N_ARG_LIST              63
#define N_ARG_LIST1             64
#define N_EXP_LST               65
#define N_EXP_LST1              66
#define N_AND_EXP               67
#define N_OR_EXP                68
#define N_RELATIONAL_EXP        69
#define N_AND_EXP1              70
#define N_SHIFT_EXP             71
#define N_RELATIONAL_EXP1       72
#define N_ADDITIVE_EXP          73
#define N_SHIFT_EXP1            74
#define N_MULT_EXP              75
#define N_ADDITIVE_EXP1         76
#define N_UNARY_EXP             77
#define N_MULT_EXP1             78
#define N_PRIMARY_EXP           79
#define N_METH_OR_ATTR          80
#define N_METH_OR_ATTR1         81

#define ACTION_BASE             82
#define A_OP_BLK_OPEN           82
#define A_OP_BLK_CLOSE          83
#define A_OP_FOR_INIT           84
#define A_OP_FOR_COND           85
#define A_OP_FOR_AFTER          86
#define A_OP_FOR_END            87
#define A_OP_WHILE_BEGIN        88
#define A_OP_WHILE_COND         89
#define A_OP_WHILE_END          90
#define A_OP_IF_COND            91
#define A_OP_IF_END             92
#define A_OP_IF_ELSE            93
#define A_OP_VAR_DECL           94
#define A_OP_PUSH_TMP           95
#define A_OP_POP_TMP            96
#define A_OP_ASSIGN             97
#define A_OP_METHOD             98
#define A_OP_METH_OR_ATTR       99
#define A_OP_RET_DISCARD       100
#define A_OP_ARG               101
#define A_OP_OR                102
#define A_OP_LOGIC_OR          103
#define A_OP_XOR               104
#define A_OP_AND               105
#define A_OP_LOGIC_AND         106
#define A_OP_EQU               107
#define A_OP_NEQ               108
#define A_OP_LT                109
#define A_OP_GT                110
#define A_OP_GTE               111
#define A_OP_LTE               112
#define A_OP_SHL               113
#define A_OP_ASR               114
#define A_OP_ADD               115
#define A_OP_SUB               116
#define A_OP_MUL               117
#define A_OP_DIV               118
#define A_OP_MOD               119
#define A_OP_INV               120
#define A_OP_MINUS             121
#define A_OP_NOT               122
#define A_OP_PUSH_INT          123
#define A_OP_PUSH_TRUE         124
#define A_OP_PUSH_FALSE        125
#define A_OP_PUSH_STRING       126

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

extern const const char * const microjs_ll_sym[];

#ifdef __cplusplus
extern "C" {
#endif

int microjs_ll_push(uint8_t * sp, unsigned int sym, unsigned int tok);
int microjs_ll_start(uint8_t * sp);

#ifdef __cplusplus
}
#endif

#endif /* __MICROJS_LL_H__ */


