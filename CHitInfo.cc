
#pragma implementation "CHitInfo.hh"

#include "config.h"
#include "CGml.hh"
#include "CModel.hh"
#include "CHitInfo.hh"

//////////////////////////////////////////////////////////////////////////////
// CHit
CHit::CHit ()
    : next(NULL), hit_type (UNKNOWN), normal (), model(NULL), sort(0)
{
}

CHit::CHit (const CHit& rhs)
    : next(NULL), hit_type (rhs.hit_type), face(rhs.face), u(rhs.u), v(rhs.v),
      normal (rhs.normal), dist(rhs.dist), model (rhs.model), sort(rhs.sort)
{
}

bool CHit::operator < (const CHit& rhs) const
{
    return dist < rhs.dist || (dist == rhs.dist && sort < rhs.sort);
}

//////////////////////////////////////////////////////////////////////////////
// CHitInfo
CHitInfo::CHitInfo ()
    : max_dist(-1.0), not_me(NULL), hits (NULL)
{
}

CHitInfo::CHitInfo (const CHitInfo &rhs)
    : ray (rhs.ray), max_dist (rhs.max_dist), not_me (rhs.not_me),
      color (rhs.color), diffc(rhs.diffc), specc(rhs.specc),
      phong (rhs.phong), hits(NULL)
{
    CHit *iter, *hit;
    for (iter = rhs.hits; iter != NULL; iter = iter->next) {
	hit = new CHit (*iter);
	add_hit (hit);
    }
}

CHitInfo::CHitInfo (test_type _type, const CRay& _ray)
    : type(_type), ray(_ray), max_dist (-1.0), not_me(NULL), hits(NULL)
{
}

CHitInfo::~CHitInfo ()
{
    // Delete hits.
    CHit *iter, *next;
    for (iter = hits; iter != NULL; iter = next) {
	next = iter->next;
	delete iter;
    }
}

CHitInfo& CHitInfo::operator = (const CHitInfo& rhs)
{
    ray = rhs.ray;
    color = rhs.color;
    diffc = rhs.diffc;
    specc = rhs.specc;
    phong = rhs.phong;

    // Delete our current set of hits and copy ones from rhs.
    CHit *iter, *next;
    for (iter = hits; iter != NULL; iter = next) {
	next = iter->next;
	delete iter;
    }
    hits = NULL;
    for (iter = rhs.hits; iter != NULL; iter = iter->next) {
	next = new CHit (*iter);
	add_hit (next);
    }
    return *this;
}

void CHitInfo::add_hit (CHit *hit)
{
#ifdef DEBUG
    assert (hit->next == NULL);
    assert (hit->model != NULL);
    assert (hit->hit_type != CHit::UNKNOWN);
#endif // def DEBUG
    
    CHit **iter = &hits;
    while (*iter != NULL && **iter < *hit)
	iter = &((*iter)->next);
    hit->next = *iter;
    *iter = hit;
}

void CHitInfo::get_surface (CGml *gml)
{
    if (hits == NULL)
	return;

#ifdef DEBUG
    // It would be nice if we kept track of this in an efficient manner so
    // that we could enable it in non-debug builds (via a command-line
    // switch).
    long stack_mark = gml->stack_size ();
#endif // def DEBUG

    gml->push (gml->valloc ((long)hits->face));
    gml->push (gml->valloc (hits->u));
    gml->push (gml->valloc (hits->v));

    gml->eval_closure (hits->model->surf);

    CScalarRef vphong, vspecc, vdiffc;
    CPointRef col;

    gml->pop (vphong);
    phong = vphong->double_val ();

    gml->pop (vspecc);
    specc = vspecc->double_val ();

    gml->pop (vdiffc);
    diffc = vdiffc->double_val ();

    gml->pop (col);
    color = *col;
#ifdef DEBUG
    assert (gml->stack_size () == stack_mark);
#endif
}

CRay CHitInfo::get_refl () const
{
    CRay refl;
    refl.origin () = intersection ();
    CPoint inormal = CPoint(0.0) - normal ();
    CPoint No = inormal * inormal.dot (ray.angle ());
    refl.angle () = ray.angle () - No - No;
    refl.angle ().normalize ();
    return refl;
}

double CHitInfo::ray_length (void) const
{
    return (hits == NULL ? -1.0 : hits->dist);
}

CPoint CHitInfo::intersection (void) const
{
    return ray.origin () + ray.angle () * ray_length ();
}

CModel *CHitInfo::model (void) const
{
    return (hits == NULL ? NULL : hits->model);
}

CPoint CHitInfo::normal (void) const
{
    return (hits == NULL ? CPoint() : hits->normal);
}

// vi:set sts=4 sw=4:
