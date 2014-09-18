#ifndef __MICROJS_LL_H__
#define __MICROJS_LL_H__

#include <stdint.h>

/* Last symbol num:140  Num rules:81 */

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
#define T_STRING                50
#define T_ERR                   51
#define T_NOT                   52

#define NONTERM_BASE            53
#define N_PROGRAM               53
#define N_STAT                  54
#define N_COMPOUND_STAT         55
#define N_STAT_LIST             56
#define N_EXP_LST_OPT           57
#define N_EXP                   58
#define N_CONDITION             59
#define N_ELSE_OPT              60
#define N_CATCH_OPT             61
#define N_VAR_LIST              62
#define N_ASSIGN_OR_CALL        63
#define N_VAR                   64
#define N_VAR_LIST1             65
#define N_VAR_ASSIGN_OPT        66
#define N_ASSIGN_OR_CALL1       67
#define N_FUNCTION_CALL         68
#define N_ARG_LIST_OPT          69
#define N_ARG_LIST              70
#define N_ARG_LIST1             71
#define N_EXP_LST               72
#define N_EXP_LST1              73
#define N_AND_EXP               74
#define N_OR_EXP                75
#define N_RELATIONAL_EXP        76
#define N_AND_EXP1              77
#define N_SHIFT_EXP             78
#define N_RELATIONAL_EXP1       79
#define N_ADDITIVE_EXP          80
#define N_SHIFT_EXP1            81
#define N_MULT_EXP              82
#define N_ADDITIVE_EXP1         83
#define N_UNARY_EXP             84
#define N_MULT_EXP1             85
#define N_PRIMARY_EXP           86
#define N_METH_OR_ATTR          87
#define N_METH_OR_ATTR1         88

#define ACTION_BASE             89
#define A_OP_BLK_OPEN           89
#define A_OP_BLK_CLOSE          90
#define A_OP_FOR_INIT           91
#define A_OP_FOR_COND           92
#define A_OP_FOR_AFTER          93
#define A_OP_FOR_END            94
#define A_OP_WHILE_BEGIN        95
#define A_OP_WHILE_COND         96
#define A_OP_WHILE_END          97
#define A_OP_IF_COND            98
#define A_OP_IF_END             99
#define A_OP_TRY_BEGIN         100
#define A_OP_THROW             101
#define A_OP_IF_ELSE           102
#define A_OP_TRY_END           103
#define A_OP_CATCH             104
#define A_OP_VAR_DECL          105
#define A_OP_PUSH_TMP          106
#define A_OP_ASSIGN            107
#define A_OP_CATCH_END         108
#define A_OP_POP_TMP           109
#define A_OP_RET_DISCARD       110
#define A_OP_METHOD            111
#define A_OP_CALL              112
#define A_OP_ARG               113
#define A_OP_OR                114
#define A_OP_LOGIC_OR          115
#define A_OP_XOR               116
#define A_OP_AND               117
#define A_OP_LOGIC_AND         118
#define A_OP_LT                119
#define A_OP_GT                120
#define A_OP_EQU               121
#define A_OP_NEQ               122
#define A_OP_GTE               123
#define A_OP_LTE               124
#define A_OP_SHL               125
#define A_OP_ASR               126
#define A_OP_ADD               127
#define A_OP_SUB               128
#define A_OP_MUL               129
#define A_OP_DIV               130
#define A_OP_MOD               131
#define A_OP_INV               132
#define A_OP_MINUS             133
#define A_OP_NOT               134
#define A_OP_PUSH_INT          135
#define A_OP_PUSH_STRING       136
#define A_OP_PUSH_TRUE         137
#define A_OP_PUSH_FALSE        138
#define A_OP_ATTR              139

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

#ifdef __cplusplus
extern "C" {
#endif

int microjs_ll_push(uint8_t * sp, unsigned int sym, unsigned int tok);
int microjs_ll_start(uint8_t * sp);

#ifdef __cplusplus
}
#endif

#endif /* __MICROJS_LL_H__ */


