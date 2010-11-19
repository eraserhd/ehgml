#ifndef CRenderer_hh
#define CRenderer_hh

#pragma interface

#include <string>
#include "CArray.hh"
#include "CPoint.hh"
#include "CClosure.hh"
#include "CHitInfo.hh"

class CRenderer {
protected:
    long ht, wid, depth;
    double fov;
    CArray *lights;
    CGml *gml;
    CModel *obj;
    CPoint amb;

    double pixel_width, image_x, image_y;
    CPoint eye;

#ifdef DEBUG
    bool trace_pel;	    // Whether or not we are tracing the current pel.
    CClosure *trace_pel_p_clos;
#endif // def DEBUG

public:
    CRenderer (CGml *_gml, const CPoint& amb, CArray *_lights, CModel *_obj,
	long _depth, double _fov, long _wid, long _ht);

    void run (const string& image_name, ostream& outf);

protected:
    void write_elem (double v, ostream& outf);
    CPoint get_pel (int x, int y);
    CPoint get_pel (CHitInfo& hit, int rlimit);
};

#endif // ndef CRenderer_hh

// vi:set sts=4 sw=4:
