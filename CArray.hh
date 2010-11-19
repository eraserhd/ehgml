#ifndef CArray_hh
#define CArray_hh

#pragma interface

#include <iostream>
#include <vector>
#include "TObjectRef.hh"
#include "CObject.hh"

class CArray : public CObject {
    friend class CGml;
public:
    static const char *type_name;
    typedef std::vector<CObject*>	object_vector_type;

protected:
    object_vector_type		object_vector;

public:
    CArray ();
    virtual ~CArray ();

    CObject * operator [] (long i) const;
    CObject *& operator [] (long i);

    long size () const;

    virtual void mark ();
    virtual std::string display_text (void) const;
};

typedef TObjectRef<CArray> CArrayRef;

#endif // ndef CArray_hh

// vi:set sts=4 sw=4 ai:
