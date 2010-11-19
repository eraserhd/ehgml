
#pragma implementation "CGmlError.hh"

#include "config.h"
#include <cstdio>
#include "CGmlError.hh"

using namespace std;

//////////////////////////////////////////////////////////////////////////////
// CGmlError
CGmlError::CGmlError ()
    : _line (-1)
{
}

CGmlError::CGmlError (const string& msgp, const string& opp,
    const string& filep, int linep)
    : _op(opp), _file(filep), _line(linep)
{
    char dbuf[25];
    if (linep != -1) {
	sprintf (dbuf, "%d", linep);
	_what = (filep == "" ? "STDIN" : filep) + ":" + dbuf + ": ";
    } else if (filep != "") {
	_what = filep + ":?: ";
    }
    if (opp != "") {
	_what += opp + ": ";
    }
    _what += msgp;
}

CGmlError::CGmlError (const CGmlError& rhs)
    : _what(rhs._what), _op(rhs._op), _file(rhs._file), _line(rhs._line)
{
}

CGmlError::~CGmlError() throw ()
{
}

CGmlError& CGmlError::operator = (const CGmlError& rhs)
{
    _what = rhs._what;
    _op = rhs._op;
    _file = rhs._file;
    _line = rhs._line;
    return *this;
}

const char *CGmlError::op (void) const 
{
    return _op.c_str ();
}

const char *CGmlError::file (void) const 
{
    return _file.c_str ();
}

int CGmlError::line (void) const
{
    return _line;
}

const char *CGmlError::what (void) const throw ()
{
    return _what.c_str ();
}

// vi:set sts=4 sw=4:
