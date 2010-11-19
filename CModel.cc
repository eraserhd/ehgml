
#pragma implementation "CModel.hh"

#include "config.h"
#include "macros.hh"
#include <cmath>
#include <string>
#include "CScalar.hh"
#include "CModel.hh"
#include "CMatrix.hh"
#include "CPoint.hh"

using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CModel

const char *CModel::type_name = "model";

CModel::CModel (CClosure *_surf)
    : surf (_surf), do_csg_hits(false)
{
}

CModel::CModel (const CModel& rhs)
    : surf (rhs.surf), transf (rhs.transf), do_csg_hits(false)
{
}

CModel::~CModel ()
{
}

CModel *CModel::translate (double x, double y, double z) const
{
    CModel *m = copy ();
    CMatrix matr;
    matr.set (3, 0, x);
    matr.set (3, 1, y);
    matr.set (3, 2, z);
    m->transf = matr * m->transf;
    return m;
}

CModel *CModel::scale (double x, double y, double z) const
{
    CModel *m = copy ();
    CMatrix matr;
    matr.set (0, 0, x);
    matr.set (1, 1, y);
    matr.set (2, 2, z);
    m->transf = matr * m->transf;
    return m;
}

CModel *CModel::rotatex (double deg) const
{
    CModel *m = copy ();
    CMatrix matr;
    deg = deg2rad( deg );
    matr.set (1, 1, cos (deg));
    matr.set (2, 1, -sin (deg));
    matr.set (1, 2, sin (deg));
    matr.set (2, 2, cos (deg));
    m->transf = matr * m->transf;
    return m;
}

CModel *CModel::rotatey (double deg) const
{
    CModel *m = copy ();
    CMatrix matr;
    deg = deg2rad( deg );
    matr.set (0, 0, cos (deg));
    matr.set (2, 0, sin (deg));
    matr.set (0, 2, -sin (deg));
    matr.set (2, 2, cos (deg));
    m->transf = matr * m->transf;
    return m;
}

CModel *CModel::rotatez (double deg) const
{
    CModel *m = copy ();
    CMatrix matr;
    deg = deg2rad( deg );
    matr.set (0, 0, cos(deg));
    matr.set (1, 0, -sin (deg));
    matr.set (0, 1, sin (deg));
    matr.set (1, 1, cos (deg));
    m->transf = matr * m->transf;
    return m;
}

void CModel::render_prep ()
{
    // Find the origin in world coordinates.
    r_origin = transf * CPoint (0,0,0);
    r_itransf = transf.inverse ();
}

bool CModel::owns (const CModel *model) const
{
    return model == this;
}

void CModel::mark ()
{
    CObject::mark ();
    if (surf != NULL)
	surf->mark ();
}

void CModel::enable_csg_hits ()
{
    do_csg_hits = true;
}

CPoint CModel::point_to_local (const CPoint& pt) const NAMED_RETURN
{
    DECLARE_RETURN_VALUE(CPoint);
    retval = r_itransf * pt;
    RETURN_DEFAULT;
}

CPoint CModel::point_to_world (const CPoint& pt) const NAMED_RETURN
{
    DECLARE_RETURN_VALUE(CPoint);
    retval = transf * pt;
    RETURN_DEFAULT;
}

CPoint CModel::vector_to_local (const CPoint& pt) const NAMED_RETURN
{
    DECLARE_RETURN_VALUE(CPoint);
    retval = r_itransf.vector_mult (pt);
    retval.normalize ();
    RETURN_DEFAULT;
}
 
CPoint CModel::vector_to_world (const CPoint& pt) const NAMED_RETURN
{
    DECLARE_RETURN_VALUE(CPoint);
    retval = transf.vector_mult (pt);
    retval.normalize ();
    RETURN_DEFAULT;
}

int CModel::quadratic_solve (double a, double b, double c, 
    double& d1, double& d2)
{
    if (a == 0.0)
	return 0;   // No solutiions - divide by zero.

    double t = b * b - 4 * a * c;
    if (t < 0.0)
	return 0;   // Solutions are imaginary.
    t = sqrt (t);

    d1 = ( -b + t ) / (2.0 * a);
    d2 = ( -b - t ) / (2.0 * a);

    return (t == 0.0 ? 1 : 2);
}

/////////////////////////////////////////////////////////////////////////////
// CPlaneModel
CPlaneModel::CPlaneModel (CClosure *_surf)
    : CModel (_surf)
{
}

CPlaneModel::CPlaneModel (const CPlaneModel& rhs)
    : CModel(rhs)
{
}

CPlaneModel::~CPlaneModel ()
{
}

string CPlaneModel::perty_name () const
{
    return "plane";
}

CModel *CPlaneModel::copy () const
{
    return new CPlaneModel (*this);
}

bool CPlaneModel::hit_test (CHitInfo& info)
{
    if (info.not_me == this)
	return false;

    CRay ray (
	    point_to_local (info.ray.origin ()),
	    vector_to_local (info.ray.angle ())
	    );
    CPoint pnormal (0.0,1.0,0.0);

    double ti = ( -(pnormal.dot (ray.origin ()) + 0.0) ) /
	(pnormal.dot (ray.angle ()));

    if (ti <= 0.0)
	return false;

    // Check to see if the hit is greater than the maximum distance.
    CPoint ourpt = ray.origin () + ray.angle () * ti;
    if (info.max_dist != -1.0) {
	double max_dist = (point_to_local (info.ray.origin () + 
		info.ray.angle () * info.max_dist) - ray.origin ()).length ();
	if (ti > max_dist)
	    return false;
    }

    CHit *hit = new CHit;
    hit->hit_type = CHit::ENTER_HIT;
    hit->face = 0;
    hit->u = ourpt.x;
    hit->v = ourpt.z;

    // FIXME: Invert this normal if necessary.
    hit->normal = r_normal;

    hit->dist = (point_to_world (ourpt) - info.ray.origin ()).length ();
    hit->model = this;
    info.add_hit (hit);

    hit = new CHit (*hit);
    hit->hit_type = CHit::EXIT_HIT;
    hit->normal = CPoint(0.0) - hit->normal;
    hit->sort ++;
    info.add_hit (hit);

    return true;
}

void CPlaneModel::render_prep ()
{
    CModel::render_prep ();
    r_normal = vector_to_world (CPoint (0.0, 1.0, 0.0));
}

/////////////////////////////////////////////////////////////////////////////
// CSphereModel
CSphereModel::CSphereModel (CClosure *_surf)
    : CModel (_surf)
{
}

CSphereModel::CSphereModel (const CSphereModel& rhs)
    : CModel (rhs)
{
}

CSphereModel::~CSphereModel ()
{
}

string CSphereModel::perty_name () const
{
    return "sphere";
}

CModel *CSphereModel::copy () const
{
    return new CSphereModel (*this);
}

bool CSphereModel::hit_test (CHitInfo& info)
{
    if (info.type != CHitInfo::SHADOW && info.not_me == this)
	return false;

    CRay ray (
	point_to_local (info.ray.origin ()),
	vector_to_local (info.ray.angle ())
	);
    CPoint pt;

    // Avoid surface acne on shadow hits.
    if (info.type == CHitInfo::SHADOW)
	ray.origin () += ray.angle () * 0.005;

    // If we originate inside the sphere, it is a shadow hit, but not a 
    // normal hit.
    double loc = ray.origin ().length ();
    if (loc <= 1.0)
	return info.type == CHitInfo::SHADOW;

    double tca = (CPoint(0.0) - ray.origin ()).dot (ray.angle ());
    if (tca <= 0.0)
	return false; // Pointing away from the sphere.

    double d = sqrt (loc * loc - tca * tca);
    if (d > 1.0)
	return false;

    // Uh, thc is never less than zero guys.  Too much thc for you ;-)
    double thc = sqrt (1.0 - d * d);
    if (info.max_dist != -1.0) {
	pt = point_to_world (ray.origin () + ray.angle () * (tca - thc));
	if ((info.ray.origin () - pt).length () > info.max_dist)
	    return false; // For shadow hits.
    }

    if (info.type == CHitInfo::SHADOW)
	return true;

    pt = ray.origin () + ray.angle () * (tca - thc);
    CHit *hit = new CHit;
    hit->hit_type = CHit::ENTER_HIT;
    hit->face = 0;
    hit->u = acos (pt.z / sqrt (1.0 - pt.y * pt.y)) / (2.0 * PI);
    hit->v = (pt.y + 1.0) / 2.0;
    hit->normal = vector_to_world (pt);
    hit->dist = (point_to_world (pt) - info.ray.origin ()).length ();
    hit->model = this;
    info.add_hit (hit);

    if (!do_csg_hits)
	return true;

    // Second point.
    pt = ray.origin () + ray.angle () * (tca + thc);
    hit = new CHit;
    hit->hit_type = CHit::EXIT_HIT;
    hit->face = 0;
    hit->u = acos (pt.z / sqrt (1.0 - pt.y * pt.y)) / (2.0 * PI);
    hit->v = (pt.y + 1.0) / 2.0;
    hit->normal = vector_to_world (pt);
    hit->dist = (point_to_world (pt) - info.ray.origin ()).length ();
    hit->model = this;
    if (thc == 0.0)
	hit->sort = 1;
    info.add_hit (hit);
    return true;
}

void CSphereModel::render_prep ()
{
    CModel::render_prep ();
}

/////////////////////////////////////////////////////////////////////////////
// CCubeModel
CCubeModel::CCubeModel (CClosure *_surf)
    : CModel (_surf)
{
}

CCubeModel::CCubeModel (const CCubeModel& rhs)
    : CModel (rhs)
{
}

CCubeModel::~CCubeModel ()
{
}

string CCubeModel::perty_name () const
{
    return "cube";
}

void CCubeModel::render_prep (void)
{
    CModel::render_prep ();
}

void CCubeModel::hit_face (int& count, CHitInfo& info, CRay& ray, int face)
{
    double d;
    CPoint normal, pt;

    switch (face) {
    case 0: // (0, u, v) = (u, v, 0), front
	d = 0.0;
	normal = CPoint(0.0, 0.0, -1.0);
	break;

    case 1: // (1, u, v) = (u, v, 1), back
	d = 1.0;
	normal = CPoint(0.0, 0.0, 1.0);
	break;

    case 2: // (2, u, v) = (0, u, v), left
	d = 0.0;
	normal = CPoint(-1.0, 0.0, 0.0);
	break;

    case 3: // (3, u, v) = (1, v, u), right
	d = 1.0;
	normal = CPoint(1.0, 0.0, 0.0);
	break;

    case 4: // (4, u, v) = (u, 1, v), top
	d = 1.0;
	normal = CPoint(0.0, 1.0, 0.0);
	break;

    case 5: // (5, u, v) = (u, 0, v), bottom
	d = 0.0;
	normal = CPoint(0.0, -1.0, 0.0);
	break;
    }

    CPoint pnormal = CPoint(0.0) - normal;

    double tid = pnormal.dot (ray.angle ());

    // Short circuit if we don't need the CSG hits.
    if (tid < 0.0 && !do_csg_hits)
	return;

    double ti = ( - (pnormal.dot (ray.origin ()) + d) / tid);

    if (ti <= 0.0)
	return;

    if (info.max_dist != -1.0) {
	pt = point_to_world (ray.origin () + ray.angle () * ti);
	if ((pt - info.ray.origin ()).length () > info.max_dist)
	    return;
    }

    // This assumes we are in local coordinates.
    pt = ray.origin () + ray.angle () * ti;
    if (normal.x == 0.0)
	if (pt.x < 0.0 || pt.x > 1.0)
	    return;
    if (normal.y == 0.0)
	if (pt.y < 0.0 || pt.y > 1.0)
	    return;
    if (normal.z == 0.0)
	if (pt.z < 0.0 || pt.z > 1.0)
	    return;

    count++;
    if (info.type == CHitInfo::SHADOW)
	return;

    CHit *hit = new CHit;
    hit->hit_type = (tid < 0.0 ? CHit::EXIT_HIT : CHit::ENTER_HIT);
    hit->face = face;
    if (face == 0 || face == 1 || face == 4 || face == 5)
	hit->u = pt.x;
    if (face == 0 || face == 1 || face == 3)
	hit->v = pt.y;
    if (face == 2)
	hit->u = pt.y;
    if (face == 2 || face == 4 || face == 5)
	hit->v = pt.z;
    if (face == 3)
	hit->u = pt.z;
    hit->normal = vector_to_world (normal);
    hit->dist = (point_to_world (pt) - info.ray.origin ()).length ();
    hit->model = this;
    info.add_hit (hit);
}

bool CCubeModel::hit_test (CHitInfo& info)
{
    int count = 0; // Optimization - stop when we've info two faces.
    CRay ray (
	    point_to_local (info.ray.origin ()),
	    vector_to_local (info.ray.angle ())
	    );

    if (info.type == CHitInfo::SHADOW) {
	// Adjustment to avoid surface acne
	//FIXME: This strikes me as a kludge (in the sphere, too) - find a
	// better method.
	ray.origin () += ray.angle () * 0.005;
    }

    hit_face (count, info, ray, 0);
    if (count == 1 && (info.type == CHitInfo::SHADOW || !do_csg_hits))
	goto done_faces;
    hit_face (count, info, ray, 1);
    if (count == 2 || (count == 1 && (info.type == CHitInfo::SHADOW || !do_csg_hits)))
	goto done_faces;
    hit_face (count, info, ray, 2);
    if (count == 2 || (count == 1 && (info.type == CHitInfo::SHADOW || !do_csg_hits)))
	goto done_faces;
    hit_face (count, info, ray, 3);
    if (count == 2 || (count == 1 && (info.type == CHitInfo::SHADOW || !do_csg_hits)))
	goto done_faces;
    hit_face (count, info, ray, 4);
    if (count == 2 || (count == 1 && (info.type == CHitInfo::SHADOW || !do_csg_hits)))
	goto done_faces;
    hit_face (count, info, ray, 5);

done_faces:
    return count != 0;
}

CModel *CCubeModel::copy () const
{
    return new CCubeModel (*this);
}

/////////////////////////////////////////////////////////////////////////////
// CConeModel
CConeModel::CConeModel (CClosure *_surf)
    : CModel (_surf)
{
}

CConeModel::CConeModel (const CConeModel& rhs)
    : CModel (rhs)
{
}

CConeModel::~CConeModel ()
{
}

string CConeModel::perty_name () const
{
    return "cone";
}

void CConeModel::render_prep (void)
{
    CModel::render_prep ();
}

bool CConeModel::hit_test (CHitInfo& info)
{
    if (info.type != CHitInfo::SHADOW && info.not_me == this)
	return false;

    CHit *hit = NULL;
    CRay ray (
	    point_to_local (info.ray.origin ()),
	    vector_to_local (info.ray.angle ())
	    );

    // Translate max_dist to our space.
    double max_dist;
    if (info.max_dist == -1.0)
	max_dist = -1.0;
    else {
	max_dist = (point_to_local (info.ray.origin () + info.ray.angle ()
		* info.max_dist) - ray.origin ()).length ();
    }

    if (info.type == CHitInfo::SHADOW) {
	// Avoid surface acne
	ray.origin () += ray.angle () * 0.005;
    }

    // Test to see if it hits the bottom cap.
    CPoint pnormal = CPoint(0.0,-1.0,0.0), hitpt;
    double tid = pnormal.dot (ray.angle ());
    if (tid >= 0.0 || do_csg_hits) {
	double ti = ( - (pnormal.dot (ray.origin ()) + 1.0) / tid);
	if (ti > 0.0 && (max_dist == -1.0 || ti <= max_dist)) {
	    hitpt = ray.origin () + ray.angle () * ti;
	    if (hitpt.x * hitpt.x + hitpt.z * hitpt.z <= 1.0) {

		if (info.type == CHitInfo::SHADOW)
		    return true;

		hit = new CHit;
		hit->hit_type = (tid >= 0.0 ? CHit::ENTER_HIT : CHit::EXIT_HIT);
		hit->normal = vector_to_world (CPoint(0.0) - pnormal);
		hit->face = 1;
		hit->u = (hitpt.x + 1.0) / 2.0;
		hit->v = (hitpt.z + 1.0) / 2.0;
		hit->dist = (point_to_world (hitpt) - info.ray.origin ()).length ();
		hit->model = this;
		info.add_hit (hit);

		if (!do_csg_hits)
		    return true;
	    }
	}
    }

    // Find points of intersection.
    double a, b, c, d1, d2;
    a = ray.angle ().x * ray.angle ().x + ray.angle ().z * ray.angle ().z
	- ray.angle ().y * ray.angle ().y;
    b = 2.0 * ( ray.angle ().x * ray.origin ().x + ray.angle ().z *
	ray.origin ().z - ray.angle ().y * ray.origin ().y );
    c = ray.origin ().x * ray.origin ().x + ray.origin ().z * ray.origin ().z
	- ray.origin ().y * ray.origin ().y;

    if ( quadratic_solve (a, b, c, d1, d2) != 0 ) {
	if (d2 >= 0.0 && (max_dist == -1.0 || d2 <= max_dist)) {
	    hitpt = ray.origin () + ray.angle () * d2;
	    if (hitpt.y >= 0.0 && hitpt.y <= 1.0) {

		if (info.type == CHitInfo::SHADOW)
		    return true;
    
		CPoint whitpt (point_to_world (hitpt));
		hit = new CHit;
		hit->hit_type = CHit::EXIT_HIT;
		hit->face = 0;
		hit->u = asin (hitpt.x) / (PI * 2.0);
		hit->v = hitpt.y;
		hit->normal = CPoint (hitpt.x, 0.0, hitpt.z);
		hit->normal.normalize ();
		hit->normal += CPoint (0.0,-1.0, 0.0);
		hit->normal.normalize ();
		hit->normal = vector_to_world (hit->normal);
		hit->dist = (whitpt - info.ray.origin ()).length ();
		hit->model = this;
		info.add_hit (hit);
	    }
	}

	if (d1 >= 0.0 && (max_dist == -1.0 || d1 <= max_dist) && do_csg_hits) {
	    hitpt = ray.origin () + ray.angle () * d1;
	    if (hitpt.y >= 0.0 && hitpt.y <= 1.0) {

		if (info.type == CHitInfo::SHADOW)
		    return true;

		CPoint whitpt = point_to_world (hitpt);
		hit = new CHit;
		hit->hit_type = CHit::ENTER_HIT;
		hit->face = 0;
		hit->u = asin (hitpt.x) / (PI * 2.0);
		hit->v = hitpt.y;
		hit->normal = CPoint (hitpt.x, 0.0, hitpt.z);
		hit->normal.normalize ();
		hit->normal += CPoint (0.0,-1.0, 0.0);
		hit->normal.normalize ();
		hit->normal = vector_to_world (hit->normal);
		hit->dist = (point_to_world (hitpt) -
		    info.ray.origin ()).length ();
		hit->model = this;
		if (d1 == d2)
		    hit->sort = 1;
		info.add_hit (hit);
	    }
	}
    }

    return hit != NULL;
}

CModel *CConeModel::copy (void) const
{
    return new CConeModel (*this);
}

/////////////////////////////////////////////////////////////////////////////
// CCylinderModel
CCylinderModel::CCylinderModel (CClosure *_surf)
    : CModel (_surf)
{
}

CCylinderModel::CCylinderModel (const CCylinderModel& rhs)
    : CModel (rhs)
{
}

CCylinderModel::~CCylinderModel ()
{
}

string CCylinderModel::perty_name () const
{
    return "cylinder";
}

void CCylinderModel::render_prep (void)
{
    CModel::render_prep ();
}

bool CCylinderModel::hit_test (CHitInfo& info)
{
    if (info.type != CHitInfo::SHADOW && info.not_me == this)
	return false;

    CHit *hit = NULL;
    CRay ray (
	    point_to_local (info.ray.origin ()),
	    vector_to_local (info.ray.angle ())
	    );

    // Translate the maximum distance to local coordinate system.
    double max_dist;
    if (info.max_dist == -1.0)
	max_dist = -1.0;
    else {
	max_dist = (point_to_local (info.ray.origin () + info.ray.angle ()
		* info.max_dist) - ray.origin ()).length ();
    }

    if (info.type == CHitInfo::SHADOW) {
	// Eliminate surface acne.
	ray.origin () += ray.angle () * 0.005;
    }

    // d is the distance from the ray's origin to the axis of the cylinder.
    double Loc = (ray.origin ()-CPoint (0.0, ray.origin ().y, 0.0)).length();
    // Test if origin is inside cylinder.
    if (ray.origin ().y >= 0.0 && ray.origin ().y <= 1.0 && Loc < 1.0)
    	return (info.type == CHitInfo::SHADOW);

    // Test to see if it hits the bottom cap.
    CPoint pnormal (0.0,1.0,0.0), hitpt;
    double tid = pnormal.dot (ray.angle ()), ti;
    if (tid >= 0.0 || do_csg_hits || info.type == CHitInfo::SHADOW) { 
	ti = ( - (pnormal.dot (ray.origin ()) + 0.0) / tid);
	if (ti > 0.0 && (max_dist == -1.0 || ti <= max_dist)) {
	    hitpt = ray.origin () + ray.angle () * ti;
	    if (hitpt.x * hitpt.x + hitpt.z * hitpt.z <= 1.0) {

		if (info.type == CHitInfo::SHADOW)
		    return true;

		hit = new CHit;
		hit->hit_type = (tid >= 0.0 ? CHit::ENTER_HIT : CHit::EXIT_HIT);
		hit->face = 1;
		hit->u = (hitpt.x + 1.0) / 2.0;
		hit->v = (hitpt.z + 1.0) / 2.0;
		hit->normal = vector_to_world (CPoint(0.0) - pnormal);
		hit->dist = (point_to_world (hitpt) - info.ray.origin ()).length ();
		hit->model = this;
		info.add_hit (hit);

		if (!do_csg_hits)
		    return true;
	    }
	}
    }

    // Test to see if it hits the top cap.
    pnormal = CPoint (0.0,-1.0,0.0);
    tid = pnormal.dot (ray.angle ());
    if (tid >= 0.0 || do_csg_hits || info.type == CHitInfo::SHADOW) {
	ti = ( - (pnormal.dot (ray.origin ()) + 1.0) / tid);
	if (ti > 0.0 && (max_dist == -1.0 || ti <= max_dist)) {
	    hitpt = ray.origin () + ray.angle () * ti;
	    if (hitpt.x * hitpt.x + hitpt.z * hitpt.z <= 1.0) {

		if (info.type == CHitInfo::SHADOW)
		    return true;

		hit = new CHit;
		hit->hit_type = (tid >= 0.0 ? CHit::ENTER_HIT : CHit::EXIT_HIT);
		hit->face = 2;
		hit->u = (hitpt.x + 1.0) / 2.0;
		hit->v = (hitpt.z + 1.0) / 2.0;
		hit->normal = vector_to_world (CPoint(0.0) - pnormal);
		hit->dist = (point_to_world (hitpt) - info.ray.origin ()).length ();
		hit->model = this;
		info.add_hit (hit);

		if (!do_csg_hits)
		    return true;
	    }
	}
    }
    
    // Find points of intersection.
    double a, b, c, d1, d2;
    a = ray.angle ().x * ray.angle ().x + ray.angle ().z * ray.angle ().z;
    b = 2.0 * (ray.angle ().x * ray.origin ().x + ray.angle ().z *
	ray.origin ().z);
    c = ray.origin ().x * ray.origin ().x + ray.origin ().z * ray.origin ().z
	- 1.0;

    if ( quadratic_solve (a, b, c, d1, d2) != 0 ) {
	if (d2 >= 0.0 && (max_dist == -1.0 || d2 <= max_dist)) {
	    hitpt = ray.origin () + ray.angle () * d2;
	    if (hitpt.y >= 0.0 && hitpt.y <= 1.0) {

		if (info.type == CHitInfo::SHADOW)
		    return true;

		hit = new CHit;
		hit->hit_type = CHit::EXIT_HIT;
		hit->face = 0;
		hit->u = asin (hitpt.x) / (PI * 2.0);
		hit->v = hitpt.y;
		hit->normal = vector_to_world (CPoint (hitpt.x, 0.0, hitpt.z));
		hit->dist = (point_to_world (hitpt) - 
		    info.ray.origin ()).length ();
		hit->model = this;
		info.add_hit (hit);
	    }
	}

	if (d1 >= 0.0 && (max_dist == -1.0 || d1 <= max_dist) && do_csg_hits) {
	    hitpt = ray.origin () + ray.angle () * d1;
	    if (hitpt.y >= 0.0 && hitpt.y <= 1.0) {

		if (info.type == CHitInfo::SHADOW)
		    return true;

		hit = new CHit;
		hit->hit_type = CHit::ENTER_HIT;
		hit->face = 0;
		hit->u = asin (hitpt.x) / (PI * 2.0);
		hit->v = hitpt.y;
		hit->normal = vector_to_world (CPoint (hitpt.x, 0.0, hitpt.z));
		hit->dist = (point_to_world (hitpt) -
		    info.ray.origin ()).length ();
		hit->model = this;
		if (d1 == d2)
		    hit->sort = 1;
		info.add_hit (hit);
	    }
	}
    }

    return (hit != NULL);
}

CModel *CCylinderModel::copy (void) const
{
    return new CCylinderModel (*this);
}

/////////////////////////////////////////////////////////////////////////////
// CCompositeModel
CCompositeModel::CCompositeModel (CModel *_model1, CModel *_model2)
    : CModel (NULL), model1 (_model1), model2 (_model2)
{
}

CCompositeModel::CCompositeModel (const CCompositeModel& rhs)
    : CModel (rhs), model1 (rhs.model1), model2 (rhs.model2)
{
}

CCompositeModel::~CCompositeModel ()
{
}

void CCompositeModel::render_prep (void)
{
    CModel::render_prep ();
    model1->render_prep ();
    model2->render_prep ();
}

bool CCompositeModel::owns (const CModel *model) const
{
    return (model == this) || model1->owns (model) || model2->owns (model);
}

void CCompositeModel::mark (void)
{
    CObject::mark ();
    if ( !model1->marked () )
	model1->mark ();
    if ( !model2->marked () )
	model2->mark ();
}

void CCompositeModel::enable_csg_hits (void)
{
    CModel::enable_csg_hits ();
    model1->enable_csg_hits ();
    model2->enable_csg_hits ();
}

/////////////////////////////////////////////////////////////////////////////
// CDifferenceModel
CDifferenceModel::CDifferenceModel (CModel *_model1, CModel *_model2)
    : CCompositeModel (_model1, _model2)
{
}

CDifferenceModel::CDifferenceModel (const CDifferenceModel& rhs)
    : CCompositeModel (rhs)
{
}

CDifferenceModel::~CDifferenceModel (void)
{
}

string CDifferenceModel::perty_name (void) const
{
    return string("difference of (") + model1->perty_name () + ") and ("
	+ model2->perty_name () + ")";
}

bool CDifferenceModel::hit_test (CHitInfo& info)
{
    CHitInfo our_info (info);

    if ( !model1->hit_test (our_info) )
	return false;

    // If we don't hit the subtracted object, all we need to do is copy the
    // hits (no processing necessary).
    if ( model2->hit_test (our_info) ) {
	CHit **iterp = &our_info.hits;
	int in_model1 = 0, in_model2 = 0;
	bool delete_flag;

	while (*iterp != NULL) {
	    delete_flag = false;
	    if (model1->owns ((*iterp)->model)) {
		switch ((*iterp)->hit_type) {
		case CHit::ENTER_HIT:
		    in_model1++;
		    break;
		case CHit::EXIT_HIT:
		    in_model1--;
		    break;
		default:
		    assert (0);
		}
		if (in_model2)
		    delete_flag = true;
	    } else if  (model2->owns ((*iterp)->model)) {
		switch ((*iterp)->hit_type) {
		case CHit::ENTER_HIT:
		    in_model2++;
		    if (in_model1) {
			(*iterp)->hit_type = CHit::EXIT_HIT;
			(*iterp)->normal = CPoint(0.0) - (*iterp)->normal;
		    }
		    break;
		case CHit::EXIT_HIT:
		    in_model2--;
		    if (in_model1) {
			(*iterp)->hit_type = CHit::ENTER_HIT;
			(*iterp)->normal = CPoint (0.0) - (*iterp)->normal;
		    }
		    break;
		default:
		    assert (0);
		}
		if (!in_model1)
		    delete_flag = true;
	    }
	    if (delete_flag) {
		CHit *old = *iterp;
		*iterp = (*iterp)->next;
		delete old;
	    } else
		iterp = &((*iterp)->next);
	}
    }

    // Add our hits to the `info' object passed to us.
    if (our_info.hits == NULL)
	return false;
    CHit *iter = our_info.hits, *next;
    while (iter != NULL) {
	next = iter->next;
	iter->next = NULL;
	info.add_hit (iter);
	iter = next;
    }
    our_info.hits = NULL; // So destructor don't fall down go boom.
    return true;
}

CModel *CDifferenceModel::copy (void) const
{
    return new CDifferenceModel (*this);
}

void CDifferenceModel::render_prep (void)
{
    CCompositeModel::render_prep ();
    enable_csg_hits ();
}

/////////////////////////////////////////////////////////////////////////////
// CIntersectionModel
CIntersectionModel::CIntersectionModel (CModel *_model1, CModel *_model2)
    : CCompositeModel (_model1, _model2)
{
}

CIntersectionModel::CIntersectionModel (const CIntersectionModel& rhs)
    : CCompositeModel (rhs)
{
}

CIntersectionModel::~CIntersectionModel (void)
{
}

string CIntersectionModel::perty_name () const
{
    return string("intersection of (") + model1->perty_name () + ") and ("
	+ model2->perty_name () + ")";
}

bool CIntersectionModel::hit_test (CHitInfo& info)
{
    CHitInfo our_info (info);
    if ( !model1->hit_test (our_info) )
	return false;
    if ( !model2->hit_test (our_info) )
	return false;

    CHit **iterp = &our_info.hits;
    int in_model1 = 0, in_model2 = 0;
    bool delete_flag;

    while (*iterp != NULL) {
	delete_flag = false;
	if (model1->owns ((*iterp)->model)) {
	    switch ((*iterp)->hit_type) {
	    case CHit::ENTER_HIT:
		in_model1++;
		break;
	    case CHit::EXIT_HIT:
		in_model1--;
		break;
	    default:
		assert (0);
	    }
	    if (!in_model2)
		delete_flag = true;
	} else if  (model2->owns ((*iterp)->model)) {
	    switch ((*iterp)->hit_type) {
	    case CHit::ENTER_HIT:
		in_model2++;
		break;
	    case CHit::EXIT_HIT:
		in_model2--;
		break;
	    default:
		assert (0);
	    }
	    if (!in_model1)
		delete_flag = true;
	}
	if (delete_flag) {
	    CHit *old = *iterp;
	    *iterp = (*iterp)->next;
	    delete old;
	} else
	    iterp = &((*iterp)->next);
    }

    // Add our hits to the `info' object passed to us.
    if (our_info.hits == NULL)
	return false;
    CHit *iter = our_info.hits, *next;
    while (iter != NULL) {
	next = iter->next;
	iter->next = NULL;
	info.add_hit (iter);
	iter = next;
    }
    our_info.hits = NULL; // So destructor don't fall down go boom.
    return true;
}

CModel *CIntersectionModel::copy (void) const
{
    return new CIntersectionModel (*this);
}

void CIntersectionModel::render_prep (void)
{
    CCompositeModel::render_prep ();
    enable_csg_hits ();
}

/////////////////////////////////////////////////////////////////////////////
// CUnionModel
CUnionModel::CUnionModel (CModel *_model1, CModel *_model2)
    : CCompositeModel (_model1, _model2)
{
}

CUnionModel::CUnionModel (const CUnionModel& rhs)
    : CCompositeModel (rhs)
{
}

CUnionModel::~CUnionModel ()
{
}

string CUnionModel::perty_name () const
{
    return string("union of (") + model1->perty_name () + ") and ("
	+ model2->perty_name () + ")";
}

CModel *CUnionModel::copy () const
{
    return new CUnionModel (*this);
}

bool CUnionModel::hit_test (CHitInfo& info)
{
    if (info.not_me == this)
	return false;

    bool b1 = model1->hit_test (info);
    if (b1 && info.type == CHitInfo::SHADOW)
	return true;

    bool b2 = model2->hit_test (info);
    return (b1 || b2);
}

// vi:set sts=4 sw=4:
