#ifndef __TYPES_LL_H__
#define __TYPES_LL_H__

#include <stdint.h>

/* Last symbol num:185  Num rules:129 */

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
#define T_INT_VAL               49
#define T_CHAR                  50
#define T_STRING                51
#define T_ERR                   52
#define T_BOOL                  53
#define T_INT                   54
#define T_FIX                   55
#define T_INT_VAR               56
#define T_INT_FUN               57
#define T_FIX_VAR               58
#define T_FIX_FUN               59
#define T_BOOL_VAR              60
#define T_BOOL_FUN              61
#define T_NOT                   62
#define T_FIX2INT               63
#define T_BOOL2INT              64
#define T_FIX_VAL               65

#define NONTERM_BASE            66
#define N_PROGRAM               66
#define N_STAT                  67
#define N_COMPOUND_STAT         68
#define N_STAT_LIST             69
#define N_FOR_LST_OPT           70
#define N_INT_EXP               71
#define N_CONDITION             72
#define N_ELSE_OPT              73
#define N_CATCH_OPT             74
#define N_BOOL_VAR_LIST         75
#define N_INT_VAR_LIST          76
#define N_FIX_VAR_LIST          77
#define N_ASSIGN_OR_CALL        78
#define N_FUNCTION_CALL         79
#define N_FIX_EXP               80
#define N_BOOL_EXP              81
#define N_ARG_LIST_OPT          82
#define N_ARG_LIST              83
#define N_EXP                   84
#define N_ARG_LIST1             85
#define N_FOR_LST               86
#define N_FOR_LST1              87
#define N_BOOL_VAR              88
#define N_BOOL_VAR_LIST1        89
#define N_BOOL_VAR_ASSIGN_OPT   90
#define N_BOOL_AND_EXP          91
#define N_BOOL_OR_EXP           92
#define N_BOOL_UNARY_EXP        93
#define N_BOOL_AND_EXP1         94
#define N_BOOL_PRI_EXP          95
#define N_REL_EXP               96
#define N_INT_REL_EXP           97
#define N_INT_VAR               98
#define N_INT_VAR_LIST1         99
#define N_INT_VAR_ASSIGN_OPT   100
#define N_INT_REL_EXP1         101
#define N_AND_EXP              102
#define N_OR_EXP               103
#define N_SHIFT_EXP            104
#define N_AND_EXP1             105
#define N_INT_ADD_EXP          106
#define N_SHIFT_EXP1           107
#define N_INT_MULT_EXP         108
#define N_INT_ADD_EXP1         109
#define N_INT_UNARY_EXP        110
#define N_INT_MULT_EXP1        111
#define N_INT_PRIMARY_EXP      112
#define N_FIX_VAR              113
#define N_FIX_VAR_LIST1        114
#define N_FIX_VAR_ASSIGN_OPT   115
#define N_FIX_REL_EXP          116
#define N_FIX_REL_EXP1         117
#define N_FIX_ADD_EXP          118
#define N_FIX_MULT_EXP         119
#define N_FIX_ADD_EXP1         120
#define N_FIX_UNARY_EXP        121
#define N_FIX_MULT_EXP1        122
#define N_FIX_PRIMARY_EXP      123

#define ACTION_BASE             124
#define A_OP_BLK_OPEN          124
#define A_OP_BLK_CLOSE         125
#define A_OP_FOR_INIT          126
#define A_OP_FOR_COND          127
#define A_OP_FOR_AFTER         128
#define A_OP_FOR_END           129
#define A_OP_WHILE_BEGIN       130
#define A_OP_WHILE_COND        131
#define A_OP_WHILE_END         132
#define A_OP_IF_COND           133
#define A_OP_IF_END            134
#define A_OP_TRY_BEGIN         135
#define A_OP_THROW             136
#define A_OP_INT_ASSIGN        137
#define A_OP_RET_DISCARD       138
#define A_OP_FIX_ASSIGN        139
#define A_OP_BOOL_ASSIGN       140
#define A_OP_METHOD            141
#define A_OP_CALL              142
#define A_OP_IF_ELSE           143
#define A_OP_TRY_END           144
#define A_OP_CATCH             145
#define A_OP_INT_VAR_DECL      146
#define A_OP_PUSH_TMP          147
#define A_OP_ASSIGN            148
#define A_OP_CATCH_END         149
#define A_OP_ARG               150
#define A_OP_VAR_DECL          151
#define A_OP_POP_TMP           152
#define A_OP_LOGIC_OR          153
#define A_OP_LOGIC_AND         154
#define A_OP_NOT               155
#define A_OP_PUSH_TRUE         156
#define A_OP_PUSH_FALSE        157
#define A_OP_LT                158
#define A_OP_GT                159
#define A_OP_EQU               160
#define A_OP_NEQ               161
#define A_OP_GTE               162
#define A_OP_LTE               163
#define A_OP_OR                164
#define A_OP_XOR               165
#define A_OP_AND               166
#define A_OP_SHL               167
#define A_OP_ASR               168
#define A_OP_ADD               169
#define A_OP_SUB               170
#define A_OP_DIV               171
#define A_OP_MOD               172
#define A_OP_MUL               173
#define A_OP_INV               174
#define A_OP_PUSH_INT          175
#define A_OP_CALL_INT_RET      176
#define A_OP_ADD_FIX           177
#define A_OP_SUB_FIX           178
#define A_OP_DIV_FIX           179
#define A_OP_MUL_FIX           180
#define A_OP_MINUS_FIX         181
#define A_OP_PUSH_FIX          182
#define A_OP_CALL_FIX_RET      183
#define A_OP_INT2FIX           184

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

extern const const char * const types_ll_sym[];

#ifdef __cplusplus
extern "C" {
#endif

int types_ll_push(uint8_t * sp, unsigned int sym, unsigned int tok);
int types_ll_start(uint8_t * sp);

#ifdef __cplusplus
}
#endif

#endif /* __TYPES_LL_H__ */


