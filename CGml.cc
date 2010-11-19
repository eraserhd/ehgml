
#pragma implementation "CGml.hh"

#include "config.h"
#include <algorithm>
#include <typeinfo>
#include <iostream>
#include "CScalar.hh"
#include "CObjectHeap.hh"
#include "CGml.hh"
#include "op.hh"
#include "TObjectRef.hh"
#include "CGmlError.hh"

CGml::CGml ()
    : function_nesting(0)
{
    // Initialize our function table.
    store ("abort", gml_abort);
    store ("acos", gml_acos);
    store ("addi", gml_addi);
    store ("addf", gml_addf);
    store ("apply", gml_apply);
    store ("asin", gml_asin);
    store ("clampf", gml_clampf);
    store ("cone", gml_cone);
    store ("cos", gml_cos);
    store ("cube", gml_cube);
    store ("cylinder", gml_cylinder);
    store ("difference", gml_difference);
    store ("divi", gml_divi);
    store ("divf", gml_divf);
    store ("eqi", gml_eqi);
    store ("eqf", gml_eqf);
    store ("floor", gml_floor);
    store ("frac", gml_frac);
    store ("get", gml_get);
    store ("getx", gml_getx);
    store ("gety", gml_gety);
    store ("getz", gml_getz);
    store ("if", gml_if);
    store ("intersect", gml_intersect);
    store ("length", gml_length);
    store ("lessi", gml_lessi);
    store ("lessf", gml_lessf);
    store ("light", gml_light);
    store ("modi", gml_modi);
    store ("muli", gml_muli);
    store ("mulf", gml_mulf);
    store ("negi", gml_negi);
    store ("negf", gml_negf);
    store ("plane", gml_plane);
    store ("point", gml_point);
    store ("pointlight", gml_pointlight);
    store ("real", gml_real);
    store ("render", gml_render);
    store ("rotatex", gml_rotatex);
    store ("rotatey", gml_rotatey);
    store ("rotatez", gml_rotatez);
    store ("scale", gml_scale);
    store ("sin", gml_sin);
    store ("sphere", gml_sphere);
    store ("spotlight", gml_spotlight);
    store ("sqrt", gml_sqrt);
    store ("subi", gml_subi);
    store ("subf", gml_subf);
    store ("translate", gml_translate);
    store ("union", gml_union);
    store ("uscale", gml_uscale);
}

CGml::~CGml ()
{
    // Remove everything from the stack.
    CObjectRef obj;
    while (stack_size () > 0)
	pop (obj);

    // Unbind everything else.
    mapper.clear ();

    // Garbage collect.  This should now get everything since nothing is bound
    // and nothing is on the stack.
    heap.gc ();
}

CScalarRef CGml::valloc (CScalar::value_type _vtype, const char *str, int len)
{
    CScalarRef val = heap.add (new CScalar ());
    val->set (_vtype, str, len);
    return val;
}

CScalarRef CGml::valloc (double dbl)
{
    CScalarRef val = heap.add (new CScalar ());
    val->set (dbl);
    return val;
}

CScalarRef CGml::valloc (long l)
{
    CScalarRef val = heap.add (new CScalar ());
    val->set (l);
    return val;
}

CScalarRef CGml::valloc (const string& str)
{
    CScalarRef val = heap.add (new CScalar ());
    val->set (str);
    return val;
}

CScalarRef CGml::valloc (bool b)
{
    CScalarRef val = heap.add (new CScalar ());
    val->set (b);
    return val;
}

CObjectRef CGml::valloc (CObject *o)
{
    return heap.add (o);
}

void CGml::eval_token (CObject *obj)
{
    CObjectRef bound;
    CClosureRef clos;
    CArrayRef arr;
    CScalar *val;
    
    // If we are in a function, take all tokens literally for the stack -
    // when we reach the outer-most function end, pop those tokens, make
    // a function value, and push the function on the stack.
    if (function_nesting > 0) {
	if (typeid (*obj) == typeid (CScalar)) {
	    val = (CScalar*)obj;
	    switch (val->type ()) {
	    case CScalar::FUNC_START:
		function_nesting++;
		break;

	    // Handle early binding.
	    case CScalar::IDENTIFIER:
		bound = lookup (val->string_val ());
		if (bound != CObjectRef())
		    obj = bound;
		break;

	    case CScalar::FUNC_END:
		// End function -- make a function object.
		function_nesting--;
		internal_pop_closure (clos);
		obj = clos;
		break;

	    default:
		break;
	    }
	}
	push (obj);
	return;
    }

    // Normal course of interpretation.
    if (typeid (*obj) == typeid (CScalar)) {
	val = (CScalar*)obj;
	switch (val->type()) {
	case CScalar::ARRAY_END:
	    internal_pop_array (arr);
	    push (arr);
	    break;

	case CScalar::BINDER:
	    pop (bound);
	    store (val->string_val (), bound);
	    break;

	case CScalar::IDENTIFIER:
	    bound = lookup (val->string_val ());
	    if (bound == NULL) 
		throw CGmlError ("not bound", val->string_val ());
	    if (typeid (*bound) == typeid (CScalar)) {
		CScalar *vb = (CScalar*) ((CObject*)bound);
		if (vb->type () == CScalar::INTERNAL_FUNCTION)
		    eval_closure (bound);
		else
		    push (bound);
	    } else
		push (bound);
	    break;

	case CScalar::INTERNAL_FUNCTION:
	    eval_closure (val);
	    break;

	case CScalar::FUNC_START:
	    function_nesting++;
	    push (val);
	    break;

	default:
	    push (val);
	    break;
	}
    } else
	push (obj);
}

void CGml::eval_closure (CObject *obj)
{
    CScalar::internal_func_type func;
    CClosure *clos;
    CScalar *cl;
    if ((clos = dynamic_cast<CClosure*> (obj)) != NULL) {
	CClosure::object_vector_type::const_iterator i;	
	for (i = clos->object_vector.begin ();
		i != clos->object_vector.end ();
		i++)
	    eval_token ((*i));
    } else if ((cl = dynamic_cast<CScalar*> (obj)) != NULL) {
	if (cl->type () == CScalar::INTERNAL_FUNCTION) {
	    func = cl->internal_func_val ();
	    (* func) (this);
	} else
	    throw CGmlError ("not a closure or operator", "apply");
    } else 
	throw CGmlError ("not a closure or operator", "apply");
}

void CGml::parse ()
{
    CScalarRef val;
    while ((val = yylex ()) != CScalarRef())
	eval_token (val);
}

// Pop values until the topmost array start mark and return them as an array.
void CGml::internal_pop_array (CArrayRef& arr)
{
    CObjectRef val;
    CScalar *v;

    arr = heap.add (new CArray ());
    while (stack_size () > 0) {
	internal_pop (val);
	if ((v = dynamic_cast<CScalar*>((CObject*)val)) != NULL) {
	    if (v->type () == CScalar::ARRAY_START)
		break;
	}
	arr->object_vector.push_back (val);
    }

    if (stack_size () == 0) {
	v = dynamic_cast<CScalar*>((CObject*)val);
	if (v == NULL || v->type () != CScalar::ARRAY_START)
	    throw CGmlError ("stack underflow", "]");
    }

    reverse (arr->object_vector.begin (), arr->object_vector.end ());
}

// Pop values until the function start mark which starts this functoin and
// return them as an function.
void CGml::internal_pop_closure (CClosureRef& clos)
{
    CObjectRef val;
    CScalar *v;

    clos = heap.add (new CClosure ());
    while (stack_size () > 0) {
	internal_pop (val);
	if ((v = dynamic_cast<CScalar*>((CObject*)val)) != NULL) {
	    if (v->type () == CScalar::FUNC_START)
		break;
	}
	clos->object_vector.push_back (val);
    }

    if (stack_size () == 0) {
	v = dynamic_cast<CScalar*>((CObject*)val);
	if (v == NULL || v->type () != CScalar::FUNC_START)
	    throw CGmlError ("stack underflow","}");
    }

    reverse (clos->object_vector.begin (), clos->object_vector.end ());
}

// Pop a number of values from the stack and make them an array.
CArrayRef& CGml::pop (CArrayRef& arr, int n)
{
    arr = heap.add (new CArray ());
    CObjectRef val;
    while (--n >= 0) {
	internal_pop (val);
	arr->object_vector.push_back (val);
    }
    reverse (arr->object_vector.begin (), arr->object_vector.end ());
    return arr;
}

void CGml::dump ()
{
    // Perform a stack dump.
    CObjectRef val;

    int i = 0;
    cerr << "Function parse nesting: " << function_nesting <<endl;
    cerr << "\nStack dump:" <<endl;
    while (stack_size () > 0) {
	internal_pop (val);
	i++;
	cerr << i << " " << *val <<endl;
    }

    cerr << "\nEnvironment:" <<endl;
    mapper_type::iterator mi;
    for (mi = mapper.begin (); mi != mapper.end (); mi++) {
	cerr << (*mi).first << " = " << *(*mi).second <<endl;
    }

    cerr << "\ndone." <<endl;
}

CObjectRef CGml::lookup (const string& name)
{
    mapper_type::iterator i = mapper.find (name);
    if (i == mapper.end ())
	return NULL;
    return (*i).second;
}

void CGml::store (const string& name, CObject *val)
{
    mapper_type::iterator i = mapper.find (name);
    if (i != mapper.end ()) {
	if (typeid (*(*i).second) == typeid(CScalar)) {
	    CScalar *value = (CScalar*) ((CObject*) (*i).second);
	    if ( value->type () == CScalar::INTERNAL_FUNCTION ) {
		cerr << name << ": cannot rebind internal function." <<endl;
		throw 1;
	    }
	}
	(*i).second = val;
	return;
    }
    mapper.insert(mapper_type::value_type (name, val));
}

void CGml::store (const string& name, CScalar::internal_func_type func)
{
    CScalarRef val = heap.add (new CScalar ());
    val->set (func);
    store (name, val);
}

void CGml::push (CObject *obj)
{
    stack.push_back (obj);
}

void CGml::internal_pop (CObjectRef& obj, const char *op)
{
    if (stack_size () == 0)
	throw CGmlError ("stack underflow", (op == NULL ? "pop" : op));
    obj = stack.back ();
    stack.pop_back ();
}

// vi:set sts=4 sw=4:
