/* $ANTLR 2.7.4: "SWD.g" -> "SWDParser.cpp"$ */
#include "SWDParser.hpp"
#include <antlr/NoViableAltException.hpp>
#include <antlr/SemanticException.hpp>
#include <antlr/ASTFactory.hpp>
#line 1 "SWD.g"
#line 8 "SWDParser.cpp"
SWDParser::SWDParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,k)
{
}

SWDParser::SWDParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,1)
{
}

SWDParser::SWDParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,k)
{
}

SWDParser::SWDParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,1)
{
}

SWDParser::SWDParser(const ANTLR_USE_NAMESPACE(antlr)ParserSharedInputState& state)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(state,1)
{
}

 RefCount<CSpecification>  SWDParser::specification() {
#line 20 "SWD.g"
	 RefCount<CSpecification> spec ;
#line 37 "SWDParser.cpp"
#line 20 "SWD.g"
	
			spec=RefCount<CSpecification>(new CSpecification);
			RefCount<CConfigurations> configs;
			RefCount<CSynchClass> cls;
		
#line 44 "SWDParser.cpp"
	
	try {      // for error handling
		configs=configurations();
#line 26 "SWD.g"
		spec->SetConfigs(configs);
#line 50 "SWDParser.cpp"
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == LITERAL_synch_class)) {
				cls=synchClassDefinition();
#line 27 "SWD.g"
				spec->AddSynchClass(cls);
#line 57 "SWDParser.cpp"
			}
			else {
				goto _loop3;
			}
			
		}
		_loop3:;
		} // ( ... )*
		match(ANTLR_USE_NAMESPACE(antlr)Token::EOF_TYPE);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_0);
	}
	return spec ;
}

 RefCount<CConfigurations>  SWDParser::configurations() {
#line 32 "SWD.g"
	 RefCount<CConfigurations> configs ;
#line 79 "SWDParser.cpp"
#line 32 "SWD.g"
	
			configs=RefCount<CConfigurations>(new CConfigurations);
			RefCount<CConfiguration> config;
		
#line 85 "SWDParser.cpp"
	
	try {      // for error handling
		match(LITERAL_config);
		match(LCURLY);
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == IDENT)) {
				config=configuration();
#line 40 "SWD.g"
				configs->AddConfig(config);
#line 96 "SWDParser.cpp"
			}
			else {
				goto _loop6;
			}
			
		}
		_loop6:;
		} // ( ... )*
		match(RCURLY);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_1);
	}
	return configs ;
}

 RefCount<CSynchClass>  SWDParser::synchClassDefinition() {
#line 60 "SWD.g"
	 RefCount<CSynchClass> cls ;
#line 118 "SWDParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  className = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 60 "SWD.g"
	
			cls=RefCount<CSynchClass>(new CSynchClass);
			RefCount<CMember> member;
		
#line 125 "SWDParser.cpp"
	
	try {      // for error handling
		match(LITERAL_synch_class);
		className = LT(1);
		match(IDENT);
#line 66 "SWD.g"
		cls->SetName(className->getText().c_str());
#line 133 "SWDParser.cpp"
		match(LCURLY);
		{
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == LITERAL_conditional || LA(1) == LITERAL_static)) {
				member=memberDefinition();
#line 69 "SWD.g"
				cls->AddMember(member);
#line 142 "SWDParser.cpp"
			}
			else {
				goto _loop14;
			}
			
		}
		_loop14:;
		} // ( ... )*
		}
		match(RCURLY);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_1);
	}
	return cls ;
}

 RefCount<CConfiguration>  SWDParser::configuration() {
#line 45 "SWD.g"
	 RefCount<CConfiguration> config ;
#line 165 "SWDParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  configName = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  configValueInt = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  configValue = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 45 "SWD.g"
	
			config=RefCount<CConfiguration>(new CConfiguration);
		
#line 173 "SWDParser.cpp"
	
	try {      // for error handling
		configName = LT(1);
		match(IDENT);
#line 50 "SWD.g"
		config->SetName(configName->getText().c_str());
#line 180 "SWDParser.cpp"
		match(ASSIGN);
		{
		switch ( LA(1)) {
		case INT:
		{
			{
			configValueInt = LT(1);
			match(INT);
#line 53 "SWD.g"
			config->SetValue(configValueInt->getText().c_str());
#line 191 "SWDParser.cpp"
			}
			break;
		}
		case IDENT:
		{
			{
			configValue = LT(1);
			match(IDENT);
#line 54 "SWD.g"
			config->SetValue(configValue->getText().c_str());
#line 202 "SWDParser.cpp"
			}
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		match(SEMI);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_2);
	}
	return config ;
}

 RefCount<CMember>  SWDParser::memberDefinition() {
#line 75 "SWD.g"
	 RefCount<CMember> member ;
#line 225 "SWDParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  type = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  name = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 75 "SWD.g"
	
			member=RefCount<CMember>(new CMember);
			RefCount<CBehavior> beh;
		
#line 233 "SWDParser.cpp"
	
	try {      // for error handling
		beh=behavior();
#line 81 "SWD.g"
		member->SetBehavior(beh);
#line 239 "SWDParser.cpp"
		type = LT(1);
		match(IDENT);
#line 82 "SWD.g"
		member->SetType(type->getText().c_str());
#line 244 "SWDParser.cpp"
		name = LT(1);
		match(IDENT);
#line 83 "SWD.g"
		member->SetName(name->getText().c_str());
#line 249 "SWDParser.cpp"
		match(SEMI);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_3);
	}
	return member ;
}

 RefCount<CBehavior>  SWDParser::behavior() {
#line 87 "SWD.g"
	 RefCount<CBehavior> beh ;
#line 263 "SWDParser.cpp"
#line 87 "SWD.g"
	
			beh=RefCount<CBehavior>(new CBehavior);
		
#line 268 "SWDParser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case LITERAL_conditional:
		{
			{
			match(LITERAL_conditional);
#line 92 "SWD.g"
			beh->SetType(BehaviorType_Conditional);
#line 278 "SWDParser.cpp"
			}
			break;
		}
		case LITERAL_static:
		{
			{
			match(LITERAL_static);
#line 93 "SWD.g"
			beh->SetType(BehaviorType_Static);
#line 288 "SWDParser.cpp"
			}
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_4);
	}
	return beh ;
}

void SWDParser::identifier() {
	
	try {      // for error handling
		match(IDENT);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_0);
	}
}

void SWDParser::initializeASTFactory( ANTLR_USE_NAMESPACE(antlr)ASTFactory& )
{
}
const char* SWDParser::tokenNames[] = {
	"<0>",
	"EOF",
	"<2>",
	"NULL_TREE_LOOKAHEAD",
	"\"config\"",
	"{",
	"}",
	"an identifer",
	"=",
	"an integer value",
	";",
	"\"synch_class\"",
	"\"conditional\"",
	"\"static\"",
	"?",
	"(",
	")",
	"[",
	"]",
	"|",
	"^",
	"&",
	":",
	",",
	".",
	"!",
	"<",
	"<<",
	">",
	">>",
	"/",
	"+",
	"-",
	"~",
	"*",
	"%",
	"::",
	"white space",
	"a preprocessor directive",
	"a comment",
	"a comment",
	"a character literal",
	"a string literal",
	"an escape sequence",
	"an escaped character value",
	"a digit",
	"an octal digit",
	"a hexadecimal digit",
	"a hexadecimal value value",
	"an floating point value",
	0
};

const unsigned long SWDParser::_tokenSet_0_data_[] = { 2UL, 0UL, 0UL, 0UL };
// EOF 
const ANTLR_USE_NAMESPACE(antlr)BitSet SWDParser::_tokenSet_0(_tokenSet_0_data_,4);
const unsigned long SWDParser::_tokenSet_1_data_[] = { 2050UL, 0UL, 0UL, 0UL };
// EOF "synch_class" 
const ANTLR_USE_NAMESPACE(antlr)BitSet SWDParser::_tokenSet_1(_tokenSet_1_data_,4);
const unsigned long SWDParser::_tokenSet_2_data_[] = { 192UL, 0UL, 0UL, 0UL };
// RCURLY IDENT 
const ANTLR_USE_NAMESPACE(antlr)BitSet SWDParser::_tokenSet_2(_tokenSet_2_data_,4);
const unsigned long SWDParser::_tokenSet_3_data_[] = { 12352UL, 0UL, 0UL, 0UL };
// RCURLY "conditional" "static" 
const ANTLR_USE_NAMESPACE(antlr)BitSet SWDParser::_tokenSet_3(_tokenSet_3_data_,4);
const unsigned long SWDParser::_tokenSet_4_data_[] = { 128UL, 0UL, 0UL, 0UL };
// IDENT 
const ANTLR_USE_NAMESPACE(antlr)BitSet SWDParser::_tokenSet_4(_tokenSet_4_data_,4);


