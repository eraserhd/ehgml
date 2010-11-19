
#pragma implementation "CObject.hh"

#include "config.h"
#include <cassert>
#include <typeinfo>
#include "TObjectRef.hh"
#include "CObject.hh"

using namespace std;

const char *CObject::type_name = "object";

CObject::CObject ()
    : _ref_count (0), _marked (false)
{
}

CObject::CObject (const CObject& rhs)
    : _ref_count (0), _marked (false)
{
    // We just need this here so the compiler doesn't try to supply a default
    // copy constructor, which would copy the reference count and confuse the
    // gc.
}

CObject::~CObject ()
{
    assert (_ref_count == 0);
}

void CObject::ref ()
{
    _ref_count++;
}

void CObject::unref ()
{
    assert (_ref_count > 0);
    _ref_count--;
}

bool CObject::referenced () const
{
    return _ref_count != 0;
}

void CObject::mark ()
{
    _marked = true;
}

void CObject::unmark ()
{
    _marked = false;
}

bool CObject::marked () const
{
    return _marked;
}

string CObject::display_text (void) const
{
    return string("<object: ") + typeid(*this).name () + ">";
}

ostream& operator << (ostream& outf, const CObject& obj)
{
    outf <<obj.display_text ();
    return outf;
}
