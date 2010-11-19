
#pragma implementation "CMatrix.hh"

#include "config.h"
#include "macros.hh"
#include <cassert>
#include "CMatrix.hh"

using namespace std;

CMatrix::CMatrix()
{
    // Initialize to the identity matrix.
    for (int x = 0; x < 4 ; x++) {
	for (int y = 0; y < 4; y++) {
	    data[x][y] = (x == y ? 1.0 : 0.0);
	}
    }
}

CMatrix::CMatrix( const CMatrix& mtrx )
{
    memcpy( data, mtrx.data, sizeof( data ) );
}

void CMatrix::set( int x, int y, double v )
{
    assert (x >= 0);
    assert (x < 4);
    assert (y >= 0);
    assert (y < 4);

    data[ x ][ y ] = v;
}

CMatrix &CMatrix::operator =( const CMatrix &mtrx )
{
    memcpy( data, mtrx.data, sizeof( data ) );
    return *this;
}

CMatrix CMatrix::operator *( const CMatrix &mtrx ) const NAMED_RETURN
{
    DECLARE_RETURN_VALUE(CMatrix);
    for ( int i = 0; i < 4; i++ ) {
	for ( int j = 0; j < 4; j++ ) {
	    retval.data[i][j] =
		data[i][0] * mtrx.data[0][j] +
		data[i][1] * mtrx.data[1][j] +
                data[i][2] * mtrx.data[2][j] +
		data[i][3] * mtrx.data[3][j];
	}
    }
    RETURN_DEFAULT;
}

CMatrix &CMatrix::operator *= ( const CMatrix &mtrx )
{
    //FIXME: This constructs/destructs an extra CMatrix object.
    return operator = (operator * (mtrx));
}

CPoint CMatrix::operator * (const CPoint& pt) const NAMED_RETURN
{
    DECLARE_RETURN_VALUE(CPoint);
    retval.x = data[0][0] * pt.x + data[1][0] * pt.y + data[2][0] * pt.z
	+ data[3][0] * 1.0;
    retval.y = data[0][1] * pt.x + data[1][1] * pt.y + data[2][1] * pt.z
	+ data[3][1] * 1.0;
    retval.z = data[0][2] * pt.x + data[1][2] * pt.y + data[2][2] * pt.z
	+ data[3][2] * 1.0;
    RETURN_DEFAULT;
}

CPoint CMatrix::vector_mult (const CPoint& pt) const NAMED_RETURN
{
    DECLARE_RETURN_VALUE(CPoint);
    retval.x = data[0][0] * pt.x + data[1][0] * pt.y + data[2][0] * pt.z;
    retval.y = data[0][1] * pt.x + data[1][1] * pt.y + data[2][1] * pt.z;
    retval.z = data[0][2] * pt.x + data[1][2] * pt.y + data[2][2] * pt.z;
    RETURN_DEFAULT;
}

// NOTE: This does not test to see if the matrix might not have an inverse.
CMatrix CMatrix::inverse () const NAMED_RETURN
{
    DECLARE_RETURN_VALUE(CMatrix);

    CMatrix orig(*this);
    int x, i, j;
    double coeff;

    // Initialize return value to identity.
    for (i = 0; i < 4; i++)
	for (j = 0; j < 4; j++)
	    retval.data[i][j] = (i == j ? 1.0 : 0.0);

    for (x = 0; x < 4; x++) {
	// We are pivoting about x,x, solving for column x.

	// Test if we need to multiply row x to obtain 1.0 in x,x
	if (orig.data[x][x] != 1.0) {
	    coeff = 1.0/orig.data[x][x];
	    for (j = 0; j < 4; j++) {
		orig.data[x][j] = coeff * orig.data[x][j];
		retval.data[x][j] = coeff * retval.data[x][j];
	    }
	}

	// For rows other than x, we add row x multiplied by a coefficient
	// to get zero here.
	for (i = 0; i < 4; i++) {
	    if (i == x)
		continue;
	    coeff = - orig.data[i][x]; 
	    for (j = 0; j < 4; j++) {
		orig.data[i][j] += coeff * orig.data[x][j];
		retval.data[i][j] += coeff * retval.data[x][j];
	    }
	}
    }

    RETURN_DEFAULT;
}

ostream& operator << (ostream& o, const CMatrix& rhs)
{
    o <<"[\n";
    for (int i = 0; i < 4; i ++) {
	o << "[ ";
	for (int j = 0; j < 4; j++) {
	    o << rhs.data[j][i] << " ";
	}
	o << "]\n";
    }
    o <<"]\n";
    return o;
}

// vi:set sts=4 sw=4:
