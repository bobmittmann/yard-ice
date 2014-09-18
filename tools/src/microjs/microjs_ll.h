#ifndef __MICROJS_LL_H__
#define __MICROJS_LL_H__

#include <stdint.h>

/* Last symbol num:141  Num rules:83 */

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
#define T_TRY                   41
#define T_CATCH                 42
#define T_THROW                 43
#define T_FINALLY               44
#define T_CONTINUE              45
#define T_BREAK                 46
#define T_RETURN                47
#define T_ID                    48
#define T_INT                   49
#define T_STRING                50
#define T_ERR                   51

#define NONTERM_BASE            52
#define N_PROGRAM               52
#define N_STAT                  53
#define N_COMPOUND_STAT         54
#define N_STAT_LIST             55
#define N_EXP_LST_OPT           56
#define N_EXP                   57
#define N_CONDITION             58
#define N_ELSE_OPT              59
#define N_CATCH_OPT             60
#define N_VAR_LIST              61
#define N_ASSIGN_OR_CALL        62
#define N_FINALLY_OPT           63
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
#define A_OP_FINALLY           109
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
#define A_OP_EQU               120
#define A_OP_NEQ               121
#define A_OP_LT                122
#define A_OP_GT                123
#define A_OP_GTE               124
#define A_OP_LTE               125
#define A_OP_SHL               126
#define A_OP_ASR               127
#define A_OP_ADD               128
#define A_OP_SUB               129
#define A_OP_MUL               130
#define A_OP_DIV               131
#define A_OP_MOD               132
#define A_OP_INV               133
#define A_OP_MINUS             134
#define A_OP_NOT               135
#define A_OP_PUSH_INT          136
#define A_OP_PUSH_TRUE         137
#define A_OP_PUSH_FALSE        138
#define A_OP_PUSH_STRING       139
#define A_OP_ATTR              140

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


