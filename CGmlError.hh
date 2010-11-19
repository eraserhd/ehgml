#ifndef CGmlError_hh
#define CGmlError_hh

#include <string>
#include <stdexcept>

#pragma interface

//////////////////////////////////////////////////////////////////////////////
// CGmlError
class CGmlError: public std::exception {
protected:
    std::string  _op;
    std::string  _file;
    int	    _line;
    std::string  _what;

public:
    CGmlError ();
    virtual ~CGmlError () throw ();
    CGmlError (const std::string& msgp, const std::string& opp = "",
	const std::string& filep = "", int linep = -1);
    CGmlError (const CGmlError& rhs);

    CGmlError& operator = (const CGmlError& rhs);

    const char *op (void) const;
    const char *file (void) const;
    int line (void) const;
    virtual const char *what (void) const throw ();
};

#endif // CGmlError_hh

// vi:set sts=4 sw=4:
