
#pragma implementation "CPoint.hh"

#include "config.h"
#include <cassert>
#include <cstdio>
#include <cmath>
#include "CPoint.hh"

using namespace std;

const char * CPoint::type_name = "point";

CPoint::CPoint()
    : CObject (), x(0.0), y(0.0), z(0.0)	
{
}

CPoint::CPoint( const CPoint& vctr )
    : CObject (vctr), x(vctr.x), y(vctr.y), z(vctr.z)
{
}

CPoint::CPoint( const double _x, const double _y, const double _z )
    : CObject (), x(_x), y(_y), z(_z)
{
}

CPoint::CPoint( const double v )
    : CObject (), x(v), y(v), z(v)
{
}

CPoint::~CPoint ()
{
}

CPoint& CPoint::operator =( const CPoint& vctr )
{
	x = vctr.x;
	y = vctr.y;
	z = vctr.z;
	return *this;
}

CPoint CPoint::operator +( const CPoint& vctr ) const NAMED_RETURN
{
    DECLARE_RETURN_VALUE(CPoint);
    retval.x = x + vctr.x;
    retval.y = y + vctr.y;
    retval.z = z + vctr.z;
    RETURN_DEFAULT;
}

CPoint CPoint::operator *( const CPoint& vctr ) const NAMED_RETURN
{
    DECLARE_RETURN_VALUE(CPoint);
    retval.x = x * vctr.x;
    retval.y = y * vctr.y;
    retval.z = z * vctr.z;
    RETURN_DEFAULT;
}

CPoint CPoint::operator *( double d ) const NAMED_RETURN
{
    DECLARE_RETURN_VALUE(CPoint);
    retval.x = x * d;
    retval.y = y * d;
    retval.z = z * d;
    RETURN_DEFAULT;
}

CPoint CPoint::operator / ( double d ) const NAMED_RETURN
{
    DECLARE_RETURN_VALUE(CPoint);
    retval.x = x / d;
    retval.y = y / d;
    retval.z = z / d;
    RETURN_DEFAULT;
}

CPoint CPoint::operator -( const CPoint& vctr ) const NAMED_RETURN
{
    DECLARE_RETURN_VALUE(CPoint);
    retval.x = x - vctr.x;
    retval.y = y - vctr.y;
    retval.z = z - vctr.z;
    RETURN_DEFAULT;
}

CPoint &CPoint::operator +=( const CPoint& vctr )
{
	x += vctr.x;
	y += vctr.y;
	z += vctr.z;
	return *this;
}

CPoint &CPoint::operator *=( const CPoint& vctr )
{
	x *= vctr.x;
	y *= vctr.y;
	z *= vctr.z;
	return *this;
}

CPoint& CPoint::operator *= ( double d )
{
	x *= d;
	y *= d;
	z *= d;
	return *this;
}

CPoint &CPoint::operator -=( const CPoint& vctr )
{
	x -= vctr.x;
	y -= vctr.y;
	z -= vctr.z;
	return *this;
}

double CPoint::length () const
{
    return sqrt(x * x + y * y + z * z);
}

CPoint& CPoint::normalize ()
{
	return operator *= (1.0/length ());
}

CPoint CPoint::normal () const NAMED_RETURN
{
    DECLARE_RETURN_VALUE(CPoint);
    double len = length ();
    retval.x = x / len;
    retval.y = y / len;
    retval.z = z / len;
    RETURN_DEFAULT;
}

double CPoint::dot (const CPoint& rhs) const
{
    return (x * rhs.x + y * rhs.y + z * rhs.z);
}

CPoint CPoint::cross_prod (const CPoint& rhs) const NAMED_RETURN
{
    DECLARE_RETURN_VALUE(CPoint);
    retval.x = y * rhs.z - z * rhs.y;
    retval.y = z * rhs.x - x * rhs.z;
    retval.z = x * rhs.y - y * rhs.x;
    RETURN_DEFAULT;
}

string CPoint::display_text (void) const
{
    char buf[80];
    sprintf (buf, "( %f, %f, %f )", x, y, z);
    return string(buf);
}

// vi:set sts=4 sw=4:
