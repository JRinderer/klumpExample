#ifndef _ERROR_H
#define _ERROR_H

#include <string>

using namespace std;

void	Abort (const string&);

void	SyntaxError (int,const string&,const string&);

void	SemanticsError (int,const string&);

#endif