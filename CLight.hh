#ifndef CLight_hh
#define CLight_hh

#include "CObject.hh"
#include "CPoint.hh"
#include "CHitInfo.hh"

#pragma interface

//////////////////////////////////////////////////////////////////////////////
// CLight

class CLight : public CObject {
public:
    static const char *type_name;

protected:
    CPoint	    color;

public:
    CLight (const CPoint& _color);

    virtual void get_info (CHitInfo& info) const = 0;
};

typedef TObjectRef<CLight> CLightRef;

//////////////////////////////////////////////////////////////////////////////
// CDirectionalLight

class CDirectionalLight : public CLight {
public:
    static const char *type_name;

protected:
    CPoint	    dir;

public:
    CDirectionalLight (const CPoint& _color, const CPoint& _dir);

    virtual void get_info (CHitInfo& info) const;
};

//////////////////////////////////////////////////////////////////////////////
// CSpotLight (Tier-3)

class CSpotLight : public CLight {
public:
    static const char *type_name;

protected:
    CPoint	    pos;
    CPoint	    at;
    double	    cutoff;
    double	    exp;

public:
    CSpotLight (const CPoint& _color, const CPoint& _pos, const CPoint& _at,
	const double& _cutoff, const double& _exp);

    virtual void get_info (CHitInfo& info) const;
};

//////////////////////////////////////////////////////////////////////////////
// CPointLight (Tier-2)

class CPointLight : public CLight {
public:
    static const char *type_name;
protected:
    CPoint	    pos;

public:
    CPointLight (const CPoint& _color, const CPoint& _pos);

    virtual void get_info (CHitInfo& info) const;
};

#endif // CLight_hh

// vi:set sts=4 sw=4:
