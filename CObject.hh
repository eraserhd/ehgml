#ifndef CObject_hh
#define CObject_hh

#pragma interface

#include <string>
#include <iostream>
#include "TObjectRef.hh"

//////////////////////////////////////////////////////////////////////////////
// CObject - basic object which can be stored on our heap.
class CObject {
public:
    static const char *type_name;
protected:
    long	_ref_count;
    bool	_marked;

public:
    CObject ();
    CObject (const CObject& rhs);
    virtual ~CObject ();
    
    void ref ();
    void unref ();
    bool referenced () const;

    virtual void mark ();
    void unmark ();
    bool marked () const;

    virtual std::string display_text (void) const;
};

typedef TObjectRef<CObject> CObjectRef;

std::ostream& operator << (std::ostream& outf, const CObject& obj);

#endif // CObject_hh

// vi:set sts=4 sw=4:
