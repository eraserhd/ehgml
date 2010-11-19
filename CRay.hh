#ifndef CRay_hh
#define CRay_hh

#include "CPoint.hh"
#include "CPoint.hh"

class CRay {
protected:
	CPoint orig;
	CPoint ang;

public:
	CRay ()
	{
	}

	CRay (const CRay& rhs)
		: orig (rhs.orig), ang (rhs.ang)
	{
	}

	CRay (const CPoint& _orig, const CPoint& _ang)
		: orig(_orig), ang(_ang)
	{
		ang.normalize ();
	}

	CRay& operator = (const CRay& rhs)
	{
		orig = rhs.orig;
		ang = rhs.ang;
		return *this;
	}

public:
	inline const CPoint& origin () const
	{ return orig; }
	inline CPoint& origin ()
	{ return orig; }
	inline const CPoint& angle () const
	{ return ang; }
	inline CPoint& angle ()
	{ return ang; }
};

#endif // ndef CRay_hh
