#ifndef CHitInfo_hh
#define CHitInfo_hh

#include "CRay.hh"
#include "CPoint.hh"

class CGml;
class CModel;

#pragma interface

//////////////////////////////////////////////////////////////////////////////
// CHit - information about one intersection of ray and solid.
class CHit {
public:
    CHit *	next;		    // next hit.
public:
    typedef enum {
	ENTER_HIT,		    // Hit on entry to solid
	EXIT_HIT,		    // Hit on exit.
	UNKNOWN			    // Initial value (must be set before
				    // adding to a CHitInfo).
    } hit_type_type;

    hit_type_type   hit_type;	    // Type of hit.
    int		    face;	    // Face this is a hit on.
    double	    u, v;	    // Surface mapping coordinates.
    CPoint	    normal;	    // Surface normal vector.
    double	    dist;	    // Distance along ray of intersection.
    CModel *	    model;	    // Model we hit.
    int		    sort;	    // Sort number (for when distance is not
				    // enough info).

public:
    CHit ();
    CHit (const CHit& rhs);

    bool operator < (const CHit& rhs) const;
};

//////////////////////////////////////////////////////////////////////////////
// CHitInfo - our collection of hits and various other baggage carried around
//            for ray tracing.
class CHitInfo {
public:
    typedef enum {
	NORMAL,
	SHADOW,
	CSG
    } test_type;

    test_type	type;

    CRay	ray;		    // Original ray.
    double	max_dist;	    // For shadow hits - don't hit if greater
				    // than this value. (-1.0 is infinite).

    CModel *	not_me;		    // Test against surface acne.

    CPoint	color;		    // Returned by the surface proc.
				    // or by light->get_info ()
    double	diffc;		    // Diffuse coefficient.
    double	specc;		    // Specular reflection coefficient.
    double	phong;		    // Phong exponent.

    CHit *	hits;		    // List of hits from closest to furthest.

public:
    CHitInfo ();
    CHitInfo (const CHitInfo& rhs);
    CHitInfo (test_type _type, const CRay& _ray);
    ~CHitInfo ();

    CHitInfo& operator = (const CHitInfo& rhs);

    CRay get_refl () const;		// Get reflecting ray.
    void get_surface (CGml *gml);	// Get surface color, phong, coeff.

    void add_hit (CHit *hit);

public:
    // 
    // This is the distance to the closest hit.
    //
    double ray_length () const;
    CPoint intersection () const;
    CModel *model () const;
    CPoint normal () const;

    //
    // These operators allow us to determine which hit is closer.
    //
    inline bool operator < (const CHitInfo& rhs) const
    {
	return ray_length () < rhs.ray_length ();
    }

    inline bool operator == (const CHitInfo& rhs) const
    {
	return ray_length () == rhs.ray_length ();
    }

    inline bool operator != (const CHitInfo& rhs) const
    {
	return ! operator == (rhs);
    }

    inline bool operator > (const CHitInfo& rhs) const
    {
	return ray_length () > rhs.ray_length ();
    }

    inline bool operator >= (const CHitInfo& rhs) const
    {
	return ray_length () >= rhs.ray_length ();
    }

    inline bool operator <= (const CHitInfo &rhs) const
    {
	return ray_length () <= rhs.ray_length ();
    }

};

#endif // ndef CHitInfo_hh

// vi:set sts=4 sw=4:
