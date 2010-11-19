#ifndef CScalar_hh
#define CScalar_hh

#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <list>
#include <vector>

#include "CObject.hh"
#include "TObjectRef.hh"
#include "CGmlError.hh"

class CGml;
class CModel;
class CLight;

#define PI 3.14159
#define deg2rad(x) ( ( (double)(x) / (double)180 ) * (double)PI )

#pragma interface

//////////////////////////////////////////////////////////////////////////////
// CScalar
class CScalar : public CObject {
public:
    static const char * type_name;
    typedef enum {
	EMPTY,		/* Slot in heap is empty. */
	STRING,
	INTEGER,
	FLOAT,
	IDENTIFIER,
	BINDER,
	BOOLEAN,
	INTERNAL_FUNCTION,

	// These are only used by the tokenizer/parser.
	ARRAY_START,
	ARRAY_END,
	FUNC_START,
	FUNC_END
    } value_type;

    typedef void (* internal_func_type) (CGml *);

protected:
    value_type vtype;

    union {
	internal_func_type  internal_func;
	long		    lval;
	double		    dval;
	char *		    sval;
    };

public:
    CScalar ();
    CScalar (internal_func_type ifunc);
    ~CScalar ();

public:
    // Accessor values.
    const char * string_val () const;
    double double_val () const;
    long long_val () const;
    bool bool_val () const;
    internal_func_type internal_func_val () const;

    inline bool empty () const
    { return (vtype == EMPTY); }
    inline value_type type () const
    { return vtype; }

    // Used by lexer to create values from input.
    CScalar& set (value_type _vtype, const char *buf, int len);
    void set (double dbl);
    void set (long l);
    void set (const std::string& str);
    void set (bool b);
    void set (internal_func_type _internal_func);

    virtual std::string display_text (void) const;

protected:
    void unset (void);

};

typedef TObjectRef<CScalar> CScalarRef;

#endif // CScalar_hh

// vi:set sts=4 sw=4:
