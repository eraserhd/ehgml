
#pragma implementation "CScalar.hh"

#include "config.h"
#include "CScalar.hh"

using namespace std;

const char *CScalar::type_name = "scalar";

CScalar::CScalar ()
    : vtype(EMPTY)
{
}

CScalar::CScalar (CScalar::internal_func_type ifunc)
    : vtype (INTERNAL_FUNCTION), internal_func (ifunc)
{
}

CScalar::~CScalar ()
{
    unset ();
}

const char *CScalar::string_val (void) const
{
    if (vtype != STRING && vtype != IDENTIFIER && vtype != BINDER)
	throw CGmlError ("type error", "get string rep");
    return sval;
}

double CScalar::double_val (void) const
{
    if (vtype != INTEGER && vtype != FLOAT)
	throw CGmlError ("type error", "get real rep");
    return (vtype == INTEGER) ? (double)lval : dval;
}

long CScalar::long_val (void) const
{
    if (vtype != INTEGER && vtype != FLOAT)
	throw CGmlError ("type error", "get long rep");
    return (vtype == INTEGER) ? lval : (long)dval;
}

bool CScalar::bool_val (void) const
{
    if (vtype != BOOLEAN)
	throw CGmlError ("type error", "get bool rep");
    return lval != 0;
}

CScalar::internal_func_type CScalar::internal_func_val (void) const
{
    if (vtype != INTERNAL_FUNCTION)
	throw CGmlError ("type error", "get operator rep");
    return internal_func;
}
CScalar& CScalar::set (CScalar::value_type _vtype, const char *buf, int len)
{
    unset ();
    vtype = _vtype;
    switch (_vtype) {
    case STRING:
	assert (len >= 2);
	sval = new char[len-1];
	memcpy (sval, buf+1, len-2);
	sval[len-2] = 0;
	break;

    case BINDER:
	assert (len >= 2 && buf[0] == '/');
	sval = new char[len];
	memcpy (sval, buf+1, len-1);
	sval[len-1] = 0;
	break;

    case IDENTIFIER:
	assert (len >= 1);
	sval = new char[len+1];
	memcpy (sval, buf, len);
	sval[len] = 0;
	break;

    case INTEGER:
	lval = atol (buf);
	break;

    case FLOAT:
	dval = strtod (buf, NULL);  
	break;

    case BOOLEAN:
	lval = (buf[0] == 't');
	break;

    case ARRAY_START:
    case ARRAY_END:
    case FUNC_START:
    case FUNC_END:
	break;

    default:
	assert (0);
    }
    return *this;
}

string CScalar::display_text (void) const
{
    char buf[80];
    switch (type ()) {
    case CScalar::BOOLEAN:
	return string(lval ? "true" : "false");

    case CScalar::INTEGER:
	sprintf (buf, "%ld", lval);
	return string(buf);

    case CScalar::BINDER:
	return string("/") + sval;

    case CScalar::IDENTIFIER:
	return sval;

    case CScalar::STRING:
	return string("\"") + sval + "\"";

    case CScalar::FLOAT:
	sprintf (buf, "%f", dval);
	return string(buf);

    case CScalar::EMPTY:
	return string("<empty>");

    case CScalar::ARRAY_START:
	return string("<[>");

    case CScalar::ARRAY_END:
	return string("<]>");

    case CScalar::FUNC_START:
	return string("<{>");

    case CScalar::FUNC_END:
	return string("<}>");

    case CScalar::INTERNAL_FUNCTION:
	return string("<internal-function>");

    default:
	return CObject::display_text ();
    }
}

void CScalar::set (double dbl)
{
    unset ();
    vtype = FLOAT;
    dval = dbl;
}

void CScalar::set (long l)
{
    unset ();
    vtype = INTEGER;
    lval = l;
}

void CScalar::set (const string& str)
{
    unset ();
    vtype = STRING;
    sval = new char[str.length ()+1];
    memcpy (sval, str.data (), str.length ());
    sval[str.length()] = 0;
}

void CScalar::set (bool b)
{
    unset ();
    vtype = BOOLEAN;
    lval = b ? 1 : 0;
}

void CScalar::set (internal_func_type _internal_func)
{
    unset ();
    vtype = INTERNAL_FUNCTION;
    internal_func = _internal_func;
}

void CScalar::unset (void)
{
    switch (vtype) {
    case STRING:
    case IDENTIFIER:
    case BINDER:
	delete[] sval;
	break;
    }
}

// vi:set sts=4 sw=4:
