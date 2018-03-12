
#include <iostream>
#include <cstdlib>
#include <string>
#include <set>
#include "scanner.h"
#include "error.h"
#include "assembler.h"

using namespace std;

//	KLUMP DECLARATIONS  ***************************************************

Lexeme		current;

set<string> ATOMICTYPE	= { "INT","REAL" };	// BOOL and STRING
set<string>	CONSTANTS	= { "NUMBER","DECIMAL","CSTRING" };

set<string> COMPOP		= { "=","<>",">","<",">=","<=" };
set<string> ADDOP		= { "+","-","OR" };
set<string> MULOP		= { "*","/","%","AND" };

//	KLUMP PRODUCTION RULES  ***********************************************

void	parseKlumpProgram          (void);
void	parseGlobalDefinitions     (void);
void	parseConstDefinitions      (void);
void	parseConstList             (void);
void	parseConst                 (void);
void    parseTypeDefinitions       (void);
void	parseTypeList              (void);
void	parseStructType            (void);
void	parseArrayType             (void);
void	parseRecordType            (void);
void	parseFldList               (void);
void    parseDclDefinitions        (void);
void	parseDclList               (void);
void	parseDclType               (void);
void	parseProcDeclarations      (void);
void	parseSignatureList         (void);
void	parseProcSignature         (void);
void	parseFormalArgs	           (void);
void	parseFormalArgList         (void);
void	parseFormalArg             (void);
void	parseCallBy                (void);
void	parseReturnType            (void);
void	parseActualArgs            (void);
void	parseActualArgList         (void);
void	parseActualArg             (void);
void	parseProcedureList         (void);
void	parseProcedure             (void);
void	parseProcHead              (void);
void	parseProcBody              (void);
void	parseStatementList         (void);
void	parseStatement             (void);
void	parseLabel                 (void);
void	parseExecutableStatement   (void);
void	parseReadStatement         (void);
void	parseWriteStatement        (void);
void	parseAssignmentStatement   (void);
void	parseCallStatement         (void);
void	parseReturnStatement       (void);
void	parseGotoStatement         (void);
void	parseEmptyStatement        (void);
void	parseCompoundStatement     (void);
void	parseIfStatement           (void);
void	parseElseClause            (void);
void	parseWhileStatement        (void);
void	parseCaseStatement         (void);
void	parseCaseList              (void);
void	parseForStatement          (void);
void	parseNextStatement         (void);
void	parseBreakStatement        (void);
void	parseExpression            (void);
void	parseComparison            (void);
void	parseSimpleExpression      (void);
void	parseTerm                  (void);
void	parseFactor                (void);
void	parseUnary                 (void);
void	parseLval                  (void);
void	parseQualifier             (void);

//	KLUMP SUPPORT ALGORITHMS  *********************************************

void    mustMatch		(const Lexeme&,const string&,bool = true);
bool    doesMatch		(const Lexeme&,const string&,bool = false);
bool	element			(const string&,const set<string>&);

//	MAIN PROGRAM  *********************************************************

int main (int argc,char* argv[])
//	main program for the KLUMP compiler
{
    initScanner();
    current = getNext();
    parseKlumpProgram();
    exit(EXIT_SUCCESS);
}

//	PRODUCTION RULES  *****************************************************

void	parseKlumpProgram (void)
// < program > --> < global_definitions > !emit_prolog
//                 < procedure_list > !emit_epilog .
{
    parseGlobalDefinitions();
    parseProcedureList();
    if (!doesMatch(current,"."))
        SyntaxError(current.getLineNo(),".",current.getValue());
}

void	parseGlobalDefinitions (void)
// < global_definitions > --> [ GLOBAL < const_definitions >
//                            < type_definitions >
//                            < dcl_definitions >
//                            < proc_declarations > ]
{
    if (!doesMatch(current,"GLOBAL"))
        return;
    current = getNext();
    parseConstDefinitions();
    parseTypeDefinitions();
    parseDclDefinitions();
    parseProcDeclarations();
}

void	parseConstDefinitions (void)
// < const_definitions > --> [ CONST < const_list > ]
{
    if (!doesMatch(current,"CONST"))
        return;
    current = getNext();
    parseConstList();
}

void	parseConstList (void)
// < const_list > --> [ IDENTIFIER : < const > ; < const_list > ]
{
    bool loop = true;
    while (loop)
    {
        loop = false;
        mustMatch(current,"IDENTIFIER");
        mustMatch(current,":");
        parseConst ();
        mustMatch(current,";");
        if (doesMatch(current,"IDENTIFIER"))	// for tail recursion
            loop = true;
    }
}

void	parseConst (void)
// < const > --> NUMBER | DECIMAL | CSTRING
{
    if (element(current.getToken(),CONSTANTS))
        current = getNext();
    else
        SyntaxError(current.getLineNo(),"< constant >",current.getToken());
}

void    parseTypeDefinitions (void)
// < type_definitions > --> [ TYPE < type_list > ]
{
    if (!doesMatch(current,"TYPE"))
        return;
    current = getNext();
    parseTypeList();
}

void	parseTypeList (void)
// < type_list > --> [ IDENTIFIER : < struct_type > ; < type_list > ]
{
    bool loop = true;
    while (loop)
    {
        loop = false;
        mustMatch(current,"IDENTIFIER");
        mustMatch(current,":");
        parseStructType();
        mustMatch(current,";");
        if (doesMatch(current,"IDENTIFIER"))	// for tail recursion
            loop = true;
    }
}

void	parseStructType (void)
// < struct_type > --> < array_type > | < record_type >
{
    if (doesMatch(current,"ARRAY"))
        parseArrayType();
    else if (doesMatch(current,"RECORD"))
        parseRecordType();
    else
        SyntaxError(current.getLineNo(),"< type_type >",current.getToken());
}

void	parseArrayType (void)
// < array_type > --> ARRAY [ NUMBER ] OF < dcl_type >
{
    current = getNext();
    mustMatch(current,"[");
    mustMatch(current,"NUMBER");
    mustMatch(current,"]");
    mustMatch(current,"OF");
    parseDclType();
}

void	parseRecordType (void)
// < record_type > --> record < fld_list > END
{
    current = getNext();
    parseTypeList();
    mustMatch(current,"END");
}

void	parseFldList (void)
// < fld_list > --> { IDENTIFIER : < dcl_type > ; }+
{
    bool loop = true;
    while (loop)
    {
        loop = false;
        mustMatch(current,"IDENTIFIER");
        mustMatch(current,":");
        parseDclType ();
        mustMatch(current,";");
        if (doesMatch(current,"IDENTIFIER"))      // for tail recursion
            loop = true;
    }

}

void    parseDclDefinitions (void)
// < dcl_definitions > --> [ DCL < dcl_list > ]
{
    if (!doesMatch(current,"DCL"))
        return;
    current = getNext();
    parseDclList();
}



void    parseDclList (void)
// < dcl_list > --> { IDENTIFIER : < dcl_type > ; }+
{
    bool loop = true;
    while (loop)
    {
        loop = false;
        mustMatch(current,"IDENTIFIER");
        mustMatch(current,":");
        parseDclType ();
        mustMatch(current,";");
        if (doesMatch(current,"IDENTIFIER"))      // for tail recursion
            loop = true;
    }
}

void    parseDclType (void)
// < dcl_type > --> < atomic > | IDENTIFIER
{
    if (element(current.getToken(),ATOMICTYPE))
        current = getNext();
    else if (doesMatch(current,"IDENTIFIER"))
        current = getNext();
    else
        SyntaxError(current.getLineNo(),"< dcl_type >",current.getToken());
}

void    parseProcDeclarations (void)
// < proc_declarations > --> [ PROC < signature_list > ]
{
    if (!doesMatch(current,"PROC"))
        return;
    current = getNext();
    parseSignatureList ();
}

void	parseSignatureList (void)
// < signature_list > --> { < proc_signature > }*
{

    bool loop = true;
    while (loop)
    {
        loop = false;
        parseProcSignature();
        if (doesMatch(current,"IDENTIFIER"))      // for tail recursion
            loop = true;
    }
}

void	parseProcSignature (void)
// < proc_signature > --> IDENTIFIER < formal_args > < return_type > ;
{
    mustMatch(current,"IDENTIFIER");
    parseFormalArgs();
    parseReturnType();
    mustMatch(current,";");
}

void    parseFormalArgs (void)
// < formal_args > --> [ ( < formal_arg_list > ) ]
{
    if (!doesMatch(current,"("))
        return;
    current = getNext();
    parseFormalArgList();
    mustMatch(current,")");
}

void    parseFormalArgList (void)
// < formal_arg_list > --> < formal_arg > { , < formal_arg > }*
{
    bool loop = true;
    while (loop)
    {
        loop = false;
        parseFormalArg();
        if (doesMatch(current,","))
        {
            loop = true;
            current = getNext();
        }
    }
}

void    parseFormalArg (void)
// < formal_arg > --> < call_by > IDENTIFIER : < dcl_type >
{
    parseCallBy();
    mustMatch(current,"IDENTIFIER");
    mustMatch(current,":");
    parseDclType();
}

void    parseCallBy (void)
// < call_by > --> [VAR ]
{
    if (doesMatch(current,"VAR"))
        current = getNext();
}

void    parseReturnType (void)
// < return_type > --> [ : < atomic > ]
{
    if (!doesMatch(current,":"))
        return;
    current = getNext();
    if (element(current.getToken(),ATOMICTYPE))
        current = getNext();
    else
        SyntaxError (current.getLineNo(),"< atomic >",current.getToken());
}

void	parseActualArgs (void)
// < actual_args > --> [ ( < actual_arg_list > ) ]
{
    if (!doesMatch(current,"("))
        return;
    current = getNext();
    parseActualArgList();
    mustMatch(current,")");
}

void	parseActualArgList (void)
// < actual_arg_list > --> < actual_arg > { , < actual_arg >}*
{
    bool loop = true;
    while (loop)
    {
        loop = false;
        parseActualArg();
        if (doesMatch(current,","))
        {
            loop = true;
            current = getNext();
        }
    }
}

void    parseActualArg (void)
// < actual_arg > --> < expression >
{
    parseExpression();
}

void	parseProcedureList (void)
// < procedure_list > --> [ < procedure > < procedure_list> ]
{
    parseProcedure();
    while (doesMatch(current,"PROCEDURE"))	// for tail recursion
        parseProcedure();
}

void	parseProcedure (void)
// < procedure > --> < proc_head > < proc_body >
{
    parseProcHead();
    parseProcBody();
}

void	parseProcHead (void)
// < proc_head > --> PROCEDURE IDENTIFIER ;
{
    mustMatch(current,"PROCEDURE");
    mustMatch(current,"IDENTIFIER");
    mustMatch(current,";");
}

void	parseProcBody (void)
// < proc_body > --> < dcl_definitions > BEGIN < statement_list > END
{
    parseDclDefinitions();
    mustMatch(current,"BEGIN");
    parseStatementList();
    mustMatch(current,"END");
}

void    parseStatementList (void)
// < statement_list > --> [ < statement > < statement_list > ]
{
    while (!doesMatch(current,"END"))	// for tail recursion
        parseStatement();
}

void    parseStatement (void)
// < statement > --> < label > < executable_statement >
{
    parseLabel();
    parseExecutableStatement();
}

void	parseLabel (void)
// < label > --> [ # NUMBER ]
{
    if (!doesMatch(current,"#"))
        return;
    current = getNext();
    mustMatch(current,"NUMBER");
}

void	parseExecutableStatement (void)
{
    if ((doesMatch(current,"READ")) || (doesMatch(current,"READLN")))
        parseReadStatement();
    else if ((doesMatch(current,"WRITE")) || (doesMatch(current,"WRITELN")))
        parseWriteStatement();
    else if (doesMatch(current,"IDENTIFIER"))
        parseAssignmentStatement();
    else if (doesMatch(current,"CALL"))
        parseCallStatement();
    else if (doesMatch(current,"RETURN"))
        parseReturnStatement();
    else if (doesMatch(current,"GOTO"))
        parseGotoStatement();
    else if (doesMatch(current,";"))
        parseEmptyStatement();
    else if (doesMatch(current,"DO"))
        parseCompoundStatement();
    else if (doesMatch(current,"IF"))
        parseIfStatement();
    else if(doesMatch(current,"WHILE"))
        parseWhileStatement();
    else if (doesMatch(current,"CASE"))
        parseCaseStatement();
    else if (doesMatch(current,"FOR"))
        parseForStatement();
    else if (doesMatch(current,"NEXT"))
        parseNextStatement();
    else if (doesMatch(current,"BREAK"))
        parseBreakStatement();
    else
        ;
}

void    parseReadStatement (void)
// < read_statement > --> { READ | READLN } < lvals > ;
{
    bool ln_flag;
    if (doesMatch(current,"READ"))
        ln_flag = false;
    else
        ln_flag = true;
    current = getNext();
    parseActualArgs();
    mustMatch(current,";");
}

void    parseWriteStatement (void)
// < write_statement > --> { WRITE | WRITELN } < actual_args > ;
{
    bool ln_flag;
    if (doesMatch(current,"WRITE"))
        ln_flag = false;
    else
        ln_flag = true;
    current = getNext();
    parseActualArgs();
    mustMatch(current,";");
}

void    parseAssignmentStatement (void)
// < assignment_statement > --> < lval > := < expression > ;
{
    parseLval();
    mustMatch(current,":=");
    parseExpression();
    mustMatch(current,";");
}

void    parseCallStatement (void)
// < call_statement > --> CALL IDENTIFIER < actual_args > ;
{
    mustMatch(current,"CALL");
    mustMatch(current,"IDENTIFIER");
    parseActualArgs();
    mustMatch(current,";");
}

void	parseReturnStatement (void)
// < return_statement > --> RETURN [ < expression > ] ;
{
    mustMatch(current,"RETURN");
    if (!doesMatch(current,";"))
        parseExpression();
    mustMatch(current,";");
}

void    parseGotoStatement (void)
// < goto_statement > --> GOTO < label > ;
{
    mustMatch(current,"GOTO");
    parseLabel();
    mustMatch(current,";");
}

void    parseEmptyStatement (void)
// < empty_statement > --> ;
{
    mustMatch(current,";");
}

void	parseCompoundStatement (void)
// < compound_statement > --> DO ; < statement_list > END ;
{
    mustMatch(current,"DO");
    mustMatch(current,";");
    parseStatementList();
    mustMatch(current,"END");
    mustMatch(current,";");
}

void    parseIfStatement (void)
// < if_statement > --> IF ( < comparison > )
//                      THEN < statement >
//                      <else_clause >
{
    mustMatch(current,"IF");
    mustMatch(current,"(");
    parseComparison();
    mustMatch(current,")");
    mustMatch(current,"THEN");
    parseStatement();
    parseElseClause();
}

void	parseElseClause (void)
// < else_clause > --> [ ELSE < statement > ]
{
    if (!doesMatch(current,"ELSE"))
        return;
    else
        current = getNext();
    parseStatement();
}

void    parseWhileStatement (void)
// < while_statement > --> WHILE ( < comparison > )
//                         < statement >
{
    mustMatch(current,"WHILE");
    mustMatch(current,"(");
    parseComparison();
    mustMatch(current,")");
    parseStatement();
}

void	parseCaseStatement (void)
// < case_statement > --> CASE ( < expression > ) < case_list >
{
    mustMatch(current,"CASE");
    mustMatch(current,"(");
    parseExpression();
    mustMatch(current,")");
    parseCaseList();
}

void	parseCaseList(void)
// < case_list > --> { < unary > NUMBER : < statement > }*
//                   | DEFAULT : < statement >
{
    while (true)
    {
        parseUnary();
        if (!doesMatch(current,"NUMBER"))
            break;
        current = getNext();
        mustMatch(current,":");
        parseStatement();
    }
    if (doesMatch(current,"DEFAULT"))
    {
        current = getNext();
        mustMatch(current,":");
        parseStatement();
    }
}

void	parseForStatement (void)
// < for_statement > --> FOR IDENTIFIER := < expression >
//                       { TO | DOWNTO } < expression > < statement >
{
    mustMatch(current,"FOR");
    mustMatch(current,"IDENTIFIER");
    mustMatch(current,":=");
    parseExpression();
    if ((doesMatch(current,"TO")) || (doesMatch(current,"DOWNTO")))
        current = getNext();
    else
        SyntaxError(current.getLineNo(),"TO / DOWNTO",current.getToken());
    parseExpression();
    parseStatement();
}

void    parseNextStatement (void)
// < next_statement > --> NEXT ;
{
    mustMatch(current,"NEXT");
    mustMatch(current,";");
}

void    parseBreakStatement (void)
// < break_statement > --> BREAK ;
{
    mustMatch(current,"BREAK");
    mustMatch(current,";");
}

void	parseExpression (void)
// < expression > --> < comparison >
{
    parseComparison();
}

void	parseComparison (void)
// < comparison > --> < simple_expression > [ < compop > < simple_expression > ]
{
    parseSimpleExpression();
    if (!element(current.getValue(),COMPOP))
        return;
    current = getNext();
    parseSimpleExpression();
}

void	parseSimpleExpression (void)
//  < simple_expression > --> < unary > < term > [ < addop > < term . }*
{
    parseUnary();
    parseTerm();
    while (element(current.getValue(),ADDOP))	// for tail recursion
    {
        current = getNext();
        parseTerm();
    }
}

void	parseTerm (void)
// < term > --> < factor > [ < mulop > < factor > }*
{
    parseFactor();
    while (element(current.getValue(),MULOP))	// for tail recursion
    {
        current = getNext();
        parseFactor();
    }
}

void	parseFactor (void)
// < factor > --> < const > | < lval > | ( < expression > )
//                | NOT < factor >
// there is no < func_ref > in order to enable recursive descent parsing!
{
    if (doesMatch(current,"IDENTIFIER"))
    {
        parseLval();
    }
    else if (doesMatch(current,"("))
    {
        current = getNext();
        parseExpression();
        mustMatch(current,")");
    }
    else if (doesMatch(current,"NOT"))
    {
        current = getNext();
        parseFactor();
    }
    else
    {
        parseConst();
    }
}

void    parseUnary (void)
// < unary > --> [ + | - ]
{
    if ((doesMatch(current,"+")) || (doesMatch(current,"-")))
        current = getNext();
}

void    parseLval (void)
// < lval > --> IDENTIFIER < qualifier > !emit_lval
{
    mustMatch(current,"IDENTIFIER",false);
    string id = current.getValue();
    current = getNext();
    parseQualifier();
}

void    parseQualifier (void)
// < qualifier > --> [ < expression > ] < qualifier >
//                   |  . IDENTIFIER ] < qualifier >
//                   | < actual_args >
{
    if (doesMatch(current,"["))
    {
        current = getNext();
        parseExpression();
        mustMatch(current,"]");
        parseQualifier();
    }
    else if (doesMatch(current,"."))
    {
        current = getNext();
        mustMatch(current,"IDENTIFIER");
        parseQualifier();
    }
    else
        parseActualArgs();
}

//	SUPPORT ALGORITHMS  ***************************************************

void    mustMatch (const Lexeme& L,const string& s,bool advance)
{
    if (matchLexeme(L,s))
    {
        if (advance)
            current = getNext();
    }
    else
        SyntaxError(L.getLineNo(),s,L.getToken());
}

bool    doesMatch (const Lexeme& L,const string& s,bool advance)
{
    if (matchLexeme(L,s))
    {
        if (advance)
            current = getNext();
        return true;
    }
    else
        return false;
}

bool	element (const string& x,const set<string>& S)
{
    return S.find(x) != S.end();
}


