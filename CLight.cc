
#pragma implementation "CLight.hh"

#include "config.h"
#include <cmath>
#include "CPoint.hh"
#include "CLight.hh"

//////////////////////////////////////////////////////////////////////////////
// CLight

const char *CLight::type_name = "light";

CLight::CLight (const CPoint& _color)
    : CObject (), color(_color)
{
}

//////////////////////////////////////////////////////////////////////////////
// CDirectionalLight

const char *CDirectionalLight::type_name = "directional light";

CDirectionalLight::CDirectionalLight (const CPoint& _color, const CPoint& _dir)
    : CLight (_color), dir(_dir)
{
    dir.normalize ();
}

void CDirectionalLight::get_info (CHitInfo& info) const
{
    info.ray.angle () = CPoint(0.0) - dir;
    info.color = color;
    info.max_dist = -1.0;
}

//////////////////////////////////////////////////////////////////////////////
// CPointLight

const char *CPointLight::type_name = "point light";

CPointLight::CPointLight (const CPoint& _color, const CPoint& _pos)
    : CLight (_color), pos(_pos)
{
}

void CPointLight::get_info (CHitInfo& info) const
{
    info.ray.angle () = pos - info.ray.origin ();
    info.max_dist = info.ray.angle ().length ();
    info.ray.angle ().normalize ();
    info.color = color * (100.0 / (99.0 + 
	info.max_dist * info.max_dist));
}

//////////////////////////////////////////////////////////////////////////////
// CSpotLight

const char *CSpotLight::type_name = "spotlight";

CSpotLight::CSpotLight (const CPoint& _color, const CPoint& _pos,
    const CPoint& _at, const double& _cutoff, const double& _exp)
    : CLight (_color), pos(_pos), at(_at), cutoff(_cutoff), exp(_exp)
{
}

void CSpotLight::get_info (CHitInfo& info) const
{
    CPoint hit = info.ray.origin ();

    info.ray.angle () = pos - info.ray.origin ();
    info.max_dist = info.ray.angle ().length ();
    info.ray.angle ().normalize ();

    CPoint r1 = pos - at, r2 = pos - hit;
    r1.normalize (); r2.normalize ();
    if (acos (r1.dot (r2)) > cutoff) {
	info.color = CPoint(0.0);
	return;
    }

    double I = pow (
		    ((at - pos) / (at - pos).length ()).dot (
			(hit - pos) / (hit - pos).length ()
		    ), exp
		);
    info.color = color * ( 100.0 * I / (99.0 + info.max_dist * info.max_dist));
}

// vi:set sts=4 sw=4:
