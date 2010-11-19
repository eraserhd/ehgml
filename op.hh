#ifndef op_hh
#define op_hh

#include "CScalar.hh"

typedef void op_t (CGml *);

op_t gml_abort;
op_t gml_acos;
op_t gml_addi;
op_t gml_addf;
op_t gml_apply;
op_t gml_asin;
op_t gml_clampf;
op_t gml_cone;
op_t gml_cos;
op_t gml_cube;
op_t gml_cylinder;
op_t gml_difference;
op_t gml_divi;
op_t gml_divf;
op_t gml_eqi;
op_t gml_eqf;
op_t gml_floor;
op_t gml_frac;
op_t gml_get;
op_t gml_getx;
op_t gml_gety;
op_t gml_getz;
op_t gml_if;
op_t gml_intersect;
op_t gml_length;
op_t gml_lessi;
op_t gml_lessf;
op_t gml_light;
op_t gml_modi;
op_t gml_muli;
op_t gml_mulf;
op_t gml_negi;
op_t gml_negf;
op_t gml_plane;
op_t gml_point;
op_t gml_pointlight;
op_t gml_real;
op_t gml_render;
op_t gml_rotatex;
op_t gml_rotatey;
op_t gml_rotatez;
op_t gml_scale;		
op_t gml_sin;
op_t gml_sphere;
op_t gml_spotlight;
op_t gml_sqrt;
op_t gml_subi;
op_t gml_subf;
op_t gml_translate;
op_t gml_union;
op_t gml_uscale;

#endif // ndef op_hh
