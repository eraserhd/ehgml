
#include "config.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include "CScalar.hh"
#include "CGml.hh"
#include "op.hh"
#include "CModel.hh"
#include "CRenderer.hh"
#include "CPoint.hh"
#include "CLight.hh"

static char *s_abort = "abort";
void gml_abort (CGml *gml)
{
    CScalarRef msg;
    gml->pop (msg);
    throw CGmlError (msg->string_val (), "abort");
}

static char *s_acos = "acos";
void gml_acos (CGml *gml)
{
    CScalarRef param;
    gml->pop (param, s_acos);
    CScalarRef ret = gml->valloc (acos (
	deg2rad(param->double_val ())
	));
    gml->push (ret);
}

static char *s_addi = "addi";
void gml_addi (CGml *gml)
{
    CScalarRef param1, param2;
    gml->pop (param1, s_addi);
    gml->pop (param2, s_addi);
    gml->push (gml->valloc (param1->long_val () + param2->long_val ()));
}

static char *s_addf = "addf";
void gml_addf (CGml *gml)
{
    CScalarRef param1, param2;
    gml->pop (param1, s_addf);
    gml->pop (param2, s_addf);
    gml->push (gml->valloc (param1->double_val () + param2->double_val ()));
}

static char *s_apply = "apply";
void gml_apply (CGml *gml)
{
    // FIXME: Save/restore environment.
    CClosureRef clos;
    gml->pop (clos, s_apply);
    gml->eval_closure (clos);
}

static char *s_asin = "asin";
void gml_asin (CGml *gml)
{
    CScalarRef param;
    gml->pop (param, s_asin);
    gml->push (gml->valloc (asin (
	deg2rad (param->double_val ())
	)));
}

static char *s_clampf = "clampf";
void gml_clampf (CGml *gml)
{
    CScalarRef param;
    gml->pop (param, s_clampf);
    double d = param->double_val ();
    if (d < 0.0)
	d = 0.0;
    else if (d > 1.0)
	d = 1.0;
    gml->push (gml->valloc (d));
}

static char *s_cone = "cone";
void gml_cone (CGml *gml)
{
    CClosureRef surf;
    gml->pop (surf, s_cone);
    gml->push (gml->valloc (new CConeModel (surf)));
}

static char *s_cos = "cos";
void gml_cos (CGml *gml)
{
    CScalarRef param, ret;
    gml->pop (param, s_cos);
    ret = gml->valloc (cos (
	deg2rad (param->double_val ())
	));
    gml->push (ret);
}

static char *s_cube = "cube";
void gml_cube (CGml *gml)
{
    CClosureRef surf;
    gml->pop (surf, s_cube);
    gml->push (gml->valloc (new CCubeModel (surf)));
}

static char *s_cylinder = "cylinder";
void gml_cylinder (CGml *gml)
{
    CClosureRef surf;
    gml->pop (surf, s_cylinder);
    gml->push (gml->valloc (new CCylinderModel (surf)));
}

static char *s_difference = "difference";
void gml_difference (CGml *gml)
{
    CModelRef model1, model2;
    gml->pop (model2, s_difference);
    gml->pop (model1, s_difference);
    gml->push (gml->valloc (new CDifferenceModel (model1, model2)));
}

static char *s_divi = "divi";
void gml_divi (CGml *gml)
{
    CScalarRef param1, param2;
    gml->pop (param1, s_divi);
    gml->pop (param2, s_divi);
    gml->push (gml->valloc (param2->long_val () / param1->long_val ()));
}

static char *s_divf = "divf";
void gml_divf (CGml *gml)
{
    CScalarRef param1, param2;
    gml->pop (param1, s_divf);
    gml->pop (param2, s_divf);
    gml->push (gml->valloc (param2->double_val () / param1->double_val ()));
}

static char *s_eqi = "eqi";
void gml_eqi (CGml *gml)
{
    CScalarRef param1, param2;
    gml->pop (param1, s_eqi);
    gml->pop (param2, s_eqi);
    gml->push (gml->valloc (param1->long_val () == param2->long_val ()));
}

static char *s_eqf = "eqf";
void gml_eqf (CGml *gml)
{
    CScalarRef param1, param2;
    gml->pop (param1, s_eqf);
    gml->pop (param2, s_eqf);
    double diff = param1->double_val () - param2->double_val ();
    gml->push (gml->valloc (diff < 0.0001 && diff > -0.0001));
}

static char *s_floor = "floor";
void gml_floor (CGml *gml)
{
    CScalarRef param;
    gml->pop (param, s_floor);
    gml->push (gml->valloc (floor (param->double_val ())));
}

static char *s_frac = "frac";
void gml_frac (CGml *gml)
{
    CScalarRef param, ret;
    double i;
    gml->pop (param, s_frac);
    gml->push (gml->valloc (modf (param->double_val (), &i)));
}

static char *s_get = "get";
void gml_get (CGml *gml)
{
    CArrayRef param;
    CScalarRef iparam;
    gml->pop (iparam, s_get);
    gml->pop (param, s_get);
    gml->push ((*param)[ iparam->long_val () ]);
}

static char *s_getx = "getx";
void gml_getx (CGml *gml)
{
    CPointRef param;
    gml->pop (param, s_getx);
    gml->push (gml->valloc (param->x));
}

static char *s_gety = "gety";
void gml_gety (CGml *gml)
{
    CPointRef param;
    gml->pop (param, s_gety);
    gml->push (gml->valloc (param->y));
}

static char *s_getz = "getz";
void gml_getz (CGml *gml)
{
    CPointRef param;
    gml->pop (param, s_getz);
    gml->push (gml->valloc (param->z));
}

static char *s_if = "if";
void gml_if (CGml *gml)
{
    CClosureRef if_clos, else_clos;
    CScalarRef val;
    gml->pop (else_clos, s_if);
    gml->pop (if_clos, s_if);
    gml->pop (val, s_if);
    if (val->bool_val ()) {
	gml->eval_closure (if_clos);
    } else {
	gml->eval_closure (else_clos);
    }
}

static char *s_intersect = "intersect";
void gml_intersect (CGml *gml)
{
    CModelRef model1, model2;
    gml->pop (model2, s_intersect);
    gml->pop (model1, s_intersect);
    gml->push (gml->valloc (new CIntersectionModel (model1, model2)));
}

static char *s_length = "length";
void gml_length (CGml *gml)
{
    CArrayRef param;
    gml->pop (param, s_length);
    gml->push (gml->valloc (param->size ()));
}

static char *s_lessi = "lessi";
void gml_lessi (CGml *gml)
{
    CScalarRef param1, param2;
    gml->pop (param1, s_lessi);
    gml->pop (param2, s_lessi);
    gml->push (gml->valloc (param2->long_val () < param1->long_val ()));
}

static char *s_lessf = "lessf";
void gml_lessf (CGml *gml)
{
    CScalarRef param1, param2, ret;
    gml->pop (param1, s_lessf);
    gml->pop (param2, s_lessf);
    gml->push (gml->valloc (param2->double_val () < param1->double_val ()));
}

static char *s_light = "light";
void gml_light (CGml *gml)
{
    CPointRef dir, color;
    gml->pop (color, s_light);
    gml->pop (dir, s_light);
    gml->push (gml->valloc (new CDirectionalLight (
		    *color, *dir
		    )));
}

static char *s_modi = "modi";
void gml_modi (CGml *gml)
{
    CScalarRef param1, param2;
    gml->pop (param1, s_modi);
    gml->pop (param2, s_modi);
    gml->push (gml->valloc (param2->long_val () % param1->long_val ()));
}

static char *s_muli = "muli";
void gml_muli (CGml *gml)
{
    CScalarRef param1, param2;
    gml->pop (param1, s_muli);
    gml->pop (param2, s_muli);
    gml->push (gml->valloc (param1->long_val () * param2->long_val ()));
}

static char *s_mulf = "mulf";
void gml_mulf (CGml *gml)
{
    CScalarRef param1, param2;
    gml->pop (param1, s_mulf);
    gml->pop (param2, s_mulf);
    gml->push (gml->valloc (param1->double_val () * param2->double_val ()));
}

static char *s_negi = "negi";
void gml_negi (CGml *gml)
{
    CScalarRef param, ret;
    gml->pop (param, s_negi);
    gml->push (gml->valloc (- param->long_val ()));
}

static char *s_negf = "negf";
void gml_negf (CGml *gml)
{
    CScalarRef param;
    gml->pop (param, s_negf);
    gml->push (gml->valloc (- param->double_val ()));
}

static char *s_plane = "plane";
void gml_plane (CGml *gml)
{
    CClosureRef surf;
    gml->pop (surf, s_plane);
    gml->push (gml->valloc (new CPlaneModel (surf)));
}

static char *s_point = "point";
void gml_point (CGml *gml)
{
    CScalarRef z, y, x;
    gml->pop (z, s_point);
    gml->pop (y, s_point);
    gml->pop (x, s_point);
    gml->push (gml->valloc (new CPoint (
		    x->double_val (),
		    y->double_val (),
		    z->double_val ()
		    )));
}

static char *s_pointlight = "pointlight";
void gml_pointlight (CGml *gml)
{
    CPointRef pos, color;
    gml->pop (color, s_pointlight);
    gml->pop (pos, s_pointlight);
    gml->push (gml->valloc (new CPointLight (*color, *pos)));
}

static char *s_real = "real";
void gml_real (CGml *gml)
{
    // This is an identity function - integers are converted to doubles
    // as requested.
}

static char *s_render = "render";
void gml_render (CGml *gml)
{
    // Pop parameters.
    CScalarRef file, ht, wid, fov, depth;
    CModelRef model;
    CArrayRef lights;
    CPointRef amb;

    gml->pop (file, s_render);
    gml->pop (ht, s_render);
    gml->pop (wid, s_render);
    gml->pop (fov, s_render);
    gml->pop (depth, s_render);
    gml->pop (model, s_render);
    gml->pop (lights, s_render);
    gml->pop (amb, s_render);

    ofstream outf (file->string_val (), (ios::out|ios::binary) );
    CRenderer renderer (gml, *amb, lights, model,
	    depth->long_val (),
	    fov->double_val (),
	    wid->long_val (),
	    ht->long_val ());
    renderer.run (file->string_val (), outf);
    outf.close ();
}

static char *s_rotatex = "rotatex";
void gml_rotatex (CGml *gml)
{
    CScalarRef deg;
    CModelRef obj;
    gml->pop (deg, s_rotatex);
    gml->pop (obj, s_rotatex);
    CModel *m = obj->rotatex (deg->double_val ());
    gml->push (gml->valloc (m));
}

static char *s_rotatey = "rotatey";
void gml_rotatey (CGml *gml)
{
    CScalarRef deg;
    CModelRef obj;
    gml->pop (deg, s_rotatey);
    gml->pop (obj, s_rotatey);
    CModel *m = obj->rotatey (deg->double_val ());
    gml->push (gml->valloc (m));
}

static char *s_rotatez = "rotatez";
void gml_rotatez (CGml *gml)
{
    CScalarRef deg;
    CModelRef obj;
    gml->pop (deg, s_rotatez);
    gml->pop (obj, s_rotatez);
    CModel *m = obj->rotatez (deg->double_val ());
    gml->push (gml->valloc (m));
}

static char *s_scale = "scale";
void gml_scale (CGml *gml)
{
    CScalarRef z, y, x;
    CModelRef obj;
    gml->pop (z, s_scale);
    gml->pop (y, s_scale);
    gml->pop (x, s_scale);
    gml->pop (obj, s_scale);
    CModel *m = obj->scale (
	x->double_val (),
	y->double_val (),
	z->double_val ()
	);
    gml->push (gml->valloc (m));
}

static char *s_sin = "sin";
void gml_sin (CGml *gml)
{
    CScalarRef param;
    gml->pop (param, s_sin);
    gml->push (gml->valloc (sin (
	deg2rad (param->double_val ())
	)));
}

static char *s_sphere = "sphere";
void gml_sphere (CGml *gml)
{
    CClosureRef surf;
    gml->pop (surf, s_sphere);
    gml->push (gml->valloc (new CSphereModel (surf)));
}

static char *s_spotlight = "spotlight";
void gml_spotlight (CGml *gml)
{
    CPointRef pos, at, color;
    CScalarRef cutoff, exp;
    gml->pop (exp, s_spotlight);
    gml->pop (cutoff, s_spotlight);
    gml->pop (color, s_spotlight);
    gml->pop (at, s_spotlight);
    gml->pop (pos, s_spotlight);
    gml->push (gml->valloc (new CSpotLight (*color, *pos, *at,
	deg2rad (cutoff->double_val ()), exp->double_val ())));
}

static char *s_sqrt = "sqrt";
void gml_sqrt (CGml *gml)
{
    CScalarRef param;
    gml->pop (param, s_sqrt);
    gml->push (gml->valloc (sqrt (param->double_val ())));
}

static char *s_subi = "subi";
void gml_subi (CGml *gml)
{
    CScalarRef param1, param2;
    gml->pop (param1, s_subi);
    gml->pop (param2, s_subi);
    gml->push (gml->valloc (param2->long_val () - param1->long_val ()));
}

static char *s_subf = "subf";
void gml_subf (CGml *gml)
{
    CScalarRef param1, param2;
    gml->pop (param1, s_subf);
    gml->pop (param2, s_subf);
    gml->push (gml->valloc (param2->double_val () - param1->double_val ()));
}

static char *s_translate = "translate";
void gml_translate (CGml *gml)
{
    CScalarRef z, y, x;
    CModelRef obj;
    gml->pop (z, s_translate);
    gml->pop (y, s_translate);
    gml->pop (x, s_translate);
    gml->pop (obj, s_translate);
    CModel *m = obj->translate (
	x->double_val (),
	y->double_val (),
	z->double_val ()
	);
    gml->push (gml->valloc (m));
}

static char *s_union = "union";
void gml_union (CGml *gml)
{
    CModelRef obj1, obj2;
    gml->pop (obj1, s_union);
    gml->pop (obj2, s_union);
    gml->push (gml->valloc (
	new CUnionModel (obj1, obj2)
	));
}

static char *s_uscale = "uscale";
void gml_uscale (CGml *gml)
{
    // Just call scale with the three arguments.
    CScalarRef param;
    gml->pop (param, s_uscale);
    gml->push (param);
    gml->push (param);
    gml->push (param);
    gml_scale (gml);
}
// vi:set sts=4 sw=4:
