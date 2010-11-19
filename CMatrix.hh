#ifndef CMatrix_hh
#define CMatrix_hh

#include <iostream>
#include "CPoint.hh"

#pragma interface

//////////////////////////////////////////////////////////////////////////////
// CMatrix
class CMatrix {
//protected:
public: // for now.
    double data[4][4];

public:
    CMatrix();
    CMatrix( const CMatrix& mtrx );

    CMatrix operator *( const CMatrix &mtrx ) const;
    CMatrix &operator *=( const CMatrix &mtrx );
    CMatrix &operator =( const CMatrix &mtrx );

    CMatrix inverse () const;

    CPoint operator * (const CPoint& pt) const;
    CPoint vector_mult (const CPoint& pt) const;

    void set( int x, int y, double v );
};

std::ostream& operator << (std::ostream& o, const CMatrix& rhs);

#endif // ndef CMatrix_hh

// vi:set sts=4 sw=4:
