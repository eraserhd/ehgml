
#pragma implementation "CClosure.hh"

#include "config.h"
#include "CClosure.hh"

using namespace std;

const char *CClosure::type_name = "closure";

CClosure::CClosure ()
{
}

CClosure::~CClosure ()
{
}

string CClosure::display_text (void) const
{
    object_vector_type::const_iterator i;
    string ret("{ ");
    for (i = object_vector.begin (); i != object_vector.end (); i++)
	ret += (*i)->display_text () + " ";
    ret += string("}");
    return ret;
}

void CClosure::mark (void)
{
    CObject::mark ();

    object_vector_type::iterator i;
    for (i = object_vector.begin (); i != object_vector.end (); i++)
	if ( (*i) != NULL )
	    (*i)->mark ();
}

// vi:set sts=4 sw=4:
