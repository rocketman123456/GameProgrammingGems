#ifndef INC_SWDParser_hpp_
#define INC_SWDParser_hpp_

#include <antlr/config.hpp>
/* $ANTLR 2.7.4: "SWD.g" -> "SWDParser.hpp"$ */
#include <antlr/TokenStream.hpp>
#include <antlr/TokenBuffer.hpp>
#include "SWDTokenTypes.hpp"
#include <antlr/LLkParser.hpp>

class CUSTOM_API SWDParser : public ANTLR_USE_NAMESPACE(antlr)LLkParser, public SWDTokenTypes
{
#line 1 "SWD.g"
#line 15 "SWDParser.hpp"
public:
	void initializeASTFactory( ANTLR_USE_NAMESPACE(antlr)ASTFactory& factory );
protected:
	SWDParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf, int k);
public:
	SWDParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf);
protected:
	SWDParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer, int k);
public:
	SWDParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer);
	SWDParser(const ANTLR_USE_NAMESPACE(antlr)ParserSharedInputState& state);
	int getNumTokens() const
	{
		return SWDParser::NUM_TOKENS;
	}
	const char* getTokenName( int type ) const
	{
		if( type > getNumTokens() ) return 0;
		return SWDParser::tokenNames[type];
	}
	const char* const* getTokenNames() const
	{
		return SWDParser::tokenNames;
	}
	public:  RefCount<CSpecification>  specification();
	public:  RefCount<CConfigurations>  configurations();
	public:  RefCount<CSynchClass>  synchClassDefinition();
	public:  RefCount<CConfiguration>  configuration();
	public:  RefCount<CMember>  memberDefinition();
	public:  RefCount<CBehavior>  behavior();
	public: void identifier();
public:
	ANTLR_USE_NAMESPACE(antlr)RefAST getAST()
	{
		return returnAST;
	}
	
protected:
	ANTLR_USE_NAMESPACE(antlr)RefAST returnAST;
private:
	static const char* tokenNames[];
#ifndef NO_STATIC_CONSTS
	static const int NUM_TOKENS = 50;
#else
	enum {
		NUM_TOKENS = 50
	};
#endif
	
	static const unsigned long _tokenSet_0_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_0;
	static const unsigned long _tokenSet_1_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_1;
	static const unsigned long _tokenSet_2_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_2;
	static const unsigned long _tokenSet_3_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_3;
	static const unsigned long _tokenSet_4_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_4;
};

#endif /*INC_SWDParser_hpp_*/
