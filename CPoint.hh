#ifndef CPoint_hh
#define CPoint_hh

#pragma interface

#include "macros.hh"
#include <iostream>
#include "CObject.hh"

class CPoint : public CObject
{
    friend class CMatrix;
public:
    static const char * type_name;
    double x, y, z;

public:
    CPoint();
    CPoint( const CPoint& vctr );
    CPoint( const double _x, const double _y, const double _z );
    CPoint( const double v );
    virtual ~CPoint ();

    CPoint &operator =( const CPoint& vctr );
    CPoint operator +( const CPoint& vctr ) const;
    CPoint operator *( const CPoint& vctr ) const;
    CPoint operator *( double d ) const;
    CPoint operator /( double d ) const;
    CPoint operator -( const CPoint& vctr ) const;
    CPoint &operator +=( const CPoint& vctr );
    CPoint &operator *=( const CPoint& vctr );
    CPoint& operator *=( double d );
    CPoint &operator -=( const CPoint& vctr );

    // Useful for vectors.
    double length () const;
    CPoint& normalize ();
    CPoint normal () const;
    double dot (const CPoint& rhs) const;
    CPoint cross_prod (const CPoint& rhs) const;

    virtual std::string display_text (void) const;
};

typedef TObjectRef<CPoint> CPointRef;

#endif // #ifndef CPoint.hh

// vi:set sts=4 sw=4:
