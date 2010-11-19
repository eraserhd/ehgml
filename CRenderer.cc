
#pragma implementation "CRenderer.hh"

#include "config.h"
#include <cmath>
#include <iostream>
#include <typeinfo>
#include "CGml.hh"
#include "CScalar.hh"
#include "CRenderer.hh"
#include "CRay.hh"
#include "CModel.hh"
#include "CLight.hh"

#ifdef DEBUG
#define D(x) x
#define DPEL(x) do { if (trace_pel) { x; cout.flush (); } } while (0)
#else
#define D(x) 
#define DPEL(x)
#endif

CRenderer::CRenderer (CGml *_gml, const CPoint& _amb, CArray *_lights,
	CModel *_obj, long _depth, double _fov, long _wid, long _ht)
    : ht (_ht), wid(_wid), depth(_depth), fov (_fov), lights (_lights), 
      gml (_gml), obj (_obj), amb(_amb), eye(0.0, 0.0, -1.0)
{
    pixel_width = 2 * tan(0.5 * (_fov * (PI / 180.0))) / (double)_wid;
    image_x = 0 - (pixel_width * ((double)_wid / 2.0));
    image_y = pixel_width * ((double)_ht / 2.0);

#ifdef DEBUG
    trace_pel = false;
    CObjectRef clos = gml->lookup (string("trace-pel-p"));
    trace_pel_p_clos = dynamic_cast<CClosure*> ((CObject*)clos);
#endif // def DEBUG
}

CPoint CRenderer::get_pel (CHitInfo& hit, int rlimit)
{
    CPoint color (0.0,0.0,0.0);

    if (!obj->hit_test (hit))
	return color;

    DPEL(cout <<"dpel: hit!" <<endl);

    hit.get_surface (gml);
    color = amb * hit.diffc;

    CPoint diffl (0.0, 0.0, 0.0);
    CPoint specl (0.0, 0.0, 0.0);
    for (int j = 0; j < lights->size (); j++) {

	CHitInfo shadow_hit;
	shadow_hit.type = CHitInfo::SHADOW;
	shadow_hit.ray.origin () = hit.intersection ();
	shadow_hit.not_me = hit.model ();

	CLight *light = dynamic_cast<CLight*> ((*lights)[j]);
	assert (light != NULL);

	light->get_info (shadow_hit);

	// Cast a shadow ray - ignore light source if the shadow ray hits an
	// object.
	DPEL (cout <<"Lj = " <<shadow_hit.ray.angle () <<endl);

	if (!obj->hit_test (shadow_hit)) {
	    DPEL (cout <<"dpel: not a shadow hit."<<endl);
	    diffl += shadow_hit.color * 
		hit.normal ().dot (shadow_hit.ray.angle ());

	    CPoint Hj = shadow_hit.ray.angle () - hit.ray.angle ();
	    Hj.normalize ();
	    DPEL (cout <<"dpel: Hj = " <<Hj <<endl);
	    DPEL (cout <<"dpel: hit.phong = " <<hit.phong <<endl);
	    specl += shadow_hit.color * pow (hit.normal ().dot (Hj), hit.phong);
	}
    }

    DPEL(cout <<"dpel: diffl = "<<diffl <<endl);
    DPEL(cout <<"dpel: hit.diffc = "<<hit.diffc <<endl);
    DPEL(cout <<"dpel: specl = "<<specl <<endl);
    DPEL(cout <<"dpel: hit.specc = "<<hit.specc <<endl);
    color += diffl * hit.diffc + specl * hit.specc;

    if (rlimit > 0) {
	CHitInfo refl_hit (CHitInfo::NORMAL, hit.get_refl ());
	refl_hit.not_me = hit.model ();
	DPEL(cout <<"dpel: reflection:" <<endl);
	color += get_pel (refl_hit, rlimit-1) * hit.specc;
	DPEL(cout <<"dpel: end reflection." <<endl);
    }

    DPEL(cout <<"dpel: color = " <<color * hit.color <<endl);
    return color * hit.color;
}

CPoint CRenderer::get_pel (int x, int y)
{
#ifdef DEBUG 
    // This is a hook to allow us to selectively trace a pel's information
    // during debugging.
    if (trace_pel_p_clos != NULL) {
	long stack_mark = gml->stack_size ();
	gml->push (gml->valloc ((long)x));
	gml->push (gml->valloc ((long)y));
	gml->eval_closure (trace_pel_p_clos);
	CScalarRef v_trace_pel;
	gml->pop (v_trace_pel);
	trace_pel = v_trace_pel->bool_val ();
	assert (stack_mark == gml->stack_size ());
    }
#endif // def DEBUG

    DPEL(cout <<"dpel: START: tracing pixel at ( " <<x <<", " <<y <<")"<<endl);

    CPoint angle (image_x + (x + 0.5) * pixel_width,
	    image_y - (y + 0.5) * pixel_width,
	    1.0);
    angle.normalize ();
    CHitInfo hit (CHitInfo::NORMAL, CRay (eye, angle));
    return get_pel (hit, depth);
}

void CRenderer::write_elem (double v, ostream& outf)
{
    unsigned char n;
    if (v < 0.0)
	v = 0.0;
    if (v > 1.0)
	v = 1.0;

    n = (unsigned char)(v * 255);
    outf.write ((const char*)&n, 1);
}

void CRenderer::run (const string& image_name, ostream& outf)
{
    long pel_total = (long)wid * (long)ht;
    long pel_count = 0;

    CPoint color;

    // Write the ppm header.
    outf << "P6\n" << wid << " " << ht << "\n255\n";

    obj->render_prep ();
    cout <<image_name;
    for (int i = image_name.length () + 1; i <28; i++)
	cout <<" ";
    cout <<"|";
    cout.flush ();
    for (int y = 0; y < ht; y++) {
	for (int x = 0; x < wid; x++) {
	    color = get_pel (x, y);
	    write_elem (color.x, outf);
	    write_elem (color.y, outf);
	    write_elem (color.z, outf);

	    //FIXME: Don't flush the stream when we don't SEGV so much ;-)
	    outf.flush ();

	    pel_count++;
	    if (pel_count >= pel_total/50) {
		cout <<"*";
		cout.flush ();
		pel_count = 0;
	    }
	}
    }

    cout <<"|"<<endl;
}

// vi:set sts=4 sw=4:
