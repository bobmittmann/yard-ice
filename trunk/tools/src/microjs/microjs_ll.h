#ifndef __MICROJS_LL_H__
#define __MICROJS_LL_H__

#include <stdint.h>

/* Last symbol num:142  Num rules:82 */

#define T_EOF                    0
#define T_DOT                    1
#define T_COMMA                  2
#define T_SEMICOLON              3
#define T_COLON                  4
#define T_PLUS                   5
#define T_MINUS                  6
#define T_STAR                   7
#define T_SLASH                  8
#define T_PERCENT                9
#define T_AMPERSAND             10
#define T_BAR                   11
#define T_CARET                 12
#define T_TILDE                 13
#define T_EXCLAM                14
#define T_QUEST                 15
#define T_EQUALS                16
#define T_LESSTHEN              17
#define T_GREATTHEN             18
#define T_LBRACKET              19
#define T_RBRACKET              20
#define T_LPAREN                21
#define T_RPAREN                22
#define T_LBRACE                23
#define T_RBRACE                24
#define T_GTE                   25
#define T_LTE                   26
#define T_EQU                   27
#define T_NEQ                   28
#define T_ASR                   29
#define T_SHL                   30
#define T_LOGICOR               31
#define T_LOGICAND              32
#define T_BREAK                 33
#define T_CATCH                 34
#define T_CONTINUE              35
#define T_ELSE                  36
#define T_FALSE                 37
#define T_FINALLY               38
#define T_FOR                   39
#define T_FUNCTION              40
#define T_IF                    41
#define T_RETURN                42
#define T_THROW                 43
#define T_TRUE                  44
#define T_TRY                   45
#define T_VAR                   46
#define T_WHILE                 47
#define T_ID                    48
#define T_INT                   49
#define T_CHAR                  50
#define T_STRING                51
#define T_ERR                   52
#define T_NOT                   53

#define NONTERM_BASE            54
#define N_PROGRAM               54
#define N_STAT                  55
#define N_COMPOUND_STAT         56
#define N_STAT_LIST             57
#define N_EXP_LST_OPT           58
#define N_EXP                   59
#define N_CONDITION             60
#define N_ELSE_OPT              61
#define N_CATCH_OPT             62
#define N_VAR_LIST              63
#define N_ASSIGN_OR_CALL        64
#define N_VAR                   65
#define N_VAR_LIST1             66
#define N_VAR_ASSIGN_OPT        67
#define N_ASSIGN_OR_CALL1       68
#define N_FUNCTION_CALL         69
#define N_ARG_LIST_OPT          70
#define N_ARG_LIST              71
#define N_ARG_LIST1             72
#define N_EXP_LST               73
#define N_EXP_LST1              74
#define N_AND_EXP               75
#define N_OR_EXP                76
#define N_RELATIONAL_EXP        77
#define N_AND_EXP1              78
#define N_SHIFT_EXP             79
#define N_RELATIONAL_EXP1       80
#define N_ADDITIVE_EXP          81
#define N_SHIFT_EXP1            82
#define N_MULT_EXP              83
#define N_ADDITIVE_EXP1         84
#define N_UNARY_EXP             85
#define N_MULT_EXP1             86
#define N_PRIMARY_EXP           87
#define N_METH_OR_ATTR          88
#define N_METH_OR_ATTR1         89

#define ACTION_BASE             90
#define A_OP_BLK_OPEN           90
#define A_OP_BLK_CLOSE          91
#define A_OP_FOR_INIT           92
#define A_OP_FOR_COND           93
#define A_OP_FOR_AFTER          94
#define A_OP_FOR_END            95
#define A_OP_WHILE_BEGIN        96
#define A_OP_WHILE_COND         97
#define A_OP_WHILE_END          98
#define A_OP_IF_COND            99
#define A_OP_IF_END            100
#define A_OP_TRY_BEGIN         101
#define A_OP_THROW             102
#define A_OP_IF_ELSE           103
#define A_OP_TRY_END           104
#define A_OP_CATCH             105
#define A_OP_VAR_DECL          106
#define A_OP_PUSH_TMP          107
#define A_OP_ASSIGN            108
#define A_OP_CATCH_END         109
#define A_OP_POP_TMP           110
#define A_OP_RET_DISCARD       111
#define A_OP_METHOD            112
#define A_OP_CALL              113
#define A_OP_ARG               114
#define A_OP_OR                115
#define A_OP_LOGIC_OR          116
#define A_OP_XOR               117
#define A_OP_AND               118
#define A_OP_LOGIC_AND         119
#define A_OP_LT                120
#define A_OP_GT                121
#define A_OP_EQU               122
#define A_OP_NEQ               123
#define A_OP_GTE               124
#define A_OP_LTE               125
#define A_OP_SHL               126
#define A_OP_ASR               127
#define A_OP_ADD               128
#define A_OP_SUB               129
#define A_OP_DIV               130
#define A_OP_MOD               131
#define A_OP_MUL               132
#define A_OP_INV               133
#define A_OP_MINUS             134
#define A_OP_NOT               135
#define A_OP_PUSH_INT          136
#define A_OP_PUSH_STRING       137
#define A_OP_PUSH_TRUE         138
#define A_OP_PUSH_FALSE        139
#define A_OP_ATTR              140
#define A_OP_CALL_RET          141

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


