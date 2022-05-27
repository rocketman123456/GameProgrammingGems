#ifndef INC_SWDTokenTypes_hpp_
#define INC_SWDTokenTypes_hpp_

/* $ANTLR 2.7.4: "SWD.g" -> "SWDTokenTypes.hpp"$ */

#ifndef CUSTOM_API
# define CUSTOM_API
#endif

#ifdef __cplusplus
struct CUSTOM_API SWDTokenTypes {
#endif
	enum {
		EOF_ = 1,
		LITERAL_config = 4,
		LCURLY = 5,
		RCURLY = 6,
		IDENT = 7,
		ASSIGN = 8,
		INT = 9,
		SEMI = 10,
		LITERAL_synch_class = 11,
		LITERAL_conditional = 12,
		LITERAL_static = 13,
		QUESTION = 14,
		LPAREN = 15,
		RPAREN = 16,
		LBRACK = 17,
		RBRACK = 18,
		OR = 19,
		XOR = 20,
		AND = 21,
		COLON = 22,
		COMMA = 23,
		DOT = 24,
		NOT = 25,
		LT_ = 26,
		LSHIFT = 27,
		GT = 28,
		RSHIFT = 29,
		DIV = 30,
		PLUS = 31,
		MINUS = 32,
		TILDE = 33,
		STAR = 34,
		MOD = 35,
		SCOPEOP = 36,
		WS_ = 37,
		PREPROC_DIRECTIVE = 38,
		SL_COMMENT = 39,
		ML_COMMENT = 40,
		CHAR_LITERAL = 41,
		STRING_LITERAL = 42,
		ESC = 43,
		VOCAB = 44,
		DIGIT = 45,
		OCTDIGIT = 46,
		HEXDIGIT = 47,
		HEX = 48,
		FLOAT = 49,
		NULL_TREE_LOOKAHEAD = 3
	};
#ifdef __cplusplus
};
#endif
#endif /*INC_SWDTokenTypes_hpp_*/
