
%{
#include "config.h"
#include <stdlib.h>
#include <iostream>
#include "TObjectRef.hh"
#include "CScalar.hh"
#include "CObjectHeap.hh"
#include "CGml.hh"

#define YY_DECL CScalarRef CGml::yylex (void)
%}

%option noyywrap
%%

	/* Comments and whitespace. */
"%"[^\n]*\n			;
[ \t\r\n\v]			;

[\[\]\{\}]			{
	switch (yytext[0]) {
	case '[': return valloc (CScalar::ARRAY_START);
	case ']': return valloc (CScalar::ARRAY_END);
	case '{': return valloc (CScalar::FUNC_START);
	case '}': return valloc (CScalar::FUNC_END);
	}
}

	/* Numbers */
-?[0-9]*\.[0-9]*[eE]-?[0-9]*	|	/* Real with exponent. */
-?[0-9]*\.[0-9]*		{	/* Real without exponent. */
	return valloc (CScalar::FLOAT, yytext, yyleng);
}
-?[0-9]*			{
	return valloc (CScalar::INTEGER, yytext, yyleng);
}

	/* Identifiers, binders. */
[a-zA-Z][a-zA-Z0-9\-_]*		{
	return valloc (CScalar::IDENTIFIER, yytext, yyleng);
}
"/"[a-zA-Z][a-zA-Z0-9\-_]*	{
	return valloc (CScalar::BINDER, yytext, yyleng);
}

	/* Strings */
\"[^\"]*\"			{
	return valloc (CScalar::STRING, yytext, yyleng);
}

	/* Boolean literals */
"true"/[^a-zA-Z0-9\-_]		{
	return valloc (CScalar::BOOLEAN, yytext, yyleng);
}
"false"/[^a-zA-Z0-9\-_]		{
	return valloc (CScalar::BOOLEAN, yytext, yyleng);
}

.				{
	cerr << "invalid character." << endl;
	exit (1);
}

<<EOF>>				return CScalarRef ();

%%
