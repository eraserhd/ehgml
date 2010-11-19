
#pragma implementation "CArray.hh"

#include "config.h"
#include "CObject.hh"
#include "CArray.hh"
#include "CGmlError.hh"

using namespace std;

const char *CArray::type_name = "array";

CArray::CArray ()
{
}

CArray::~CArray ()
{
}

CObject * CArray::operator [] (long i) const
{
    if (i < 0 || i >= size ())
	throw CGmlError("array index out of range", "get");
    return object_vector[i];
}

CObject *& CArray::operator [] (long i)
{
    if (i < 0 || i >= size ())
	throw CGmlError("array index out of range", "get");
    return object_vector[i];
}

long CArray::size () const
{
    return object_vector.size ();
}

void CArray::mark ()
{
    CObject::mark ();

    object_vector_type::iterator i;
    for (i = object_vector.begin (); i != object_vector.end (); i++)
	if ( (*i) != NULL )
	    (*i)->mark ();
}

string CArray::display_text (void) const
{
    string str("[");
    object_vector_type::const_iterator i;
    for (i = object_vector.begin (); i != object_vector.end (); i++)
	str += string(" ") + (*i)->display_text ();
    if (object_vector.size () > 0)
	str += string(" ");
    str += string("]");
    return str;
}

// vi:set sts=4 sw=4:
