#ifndef CClosure_hh
#define CClosure_hh

#include <vector>
#include <iostream>
#include "CObject.hh"

#pragma interface

class CClosure : public CObject {
    friend class CGml;
public:
    static const char *type_name;
    typedef std::vector<CObject*> object_vector_type;

protected:
    object_vector_type object_vector;
    
public:
    CClosure ();
    virtual ~CClosure ();

    virtual std::string display_text (void) const;
    virtual void mark (void);
};

typedef TObjectRef<CClosure> CClosureRef;

#endif // ndef CClosure_hh

// vi:set sts=4 sw=4:
