#ifndef CGml_hh
#define CGml_hh

#include <typeinfo>
#include <iostream>
#include <functional>
#include <deque>
#include <utility>
#include <string>
#include <ext/hash_map>
#include <map>
#include "CObject.hh"
#include "CArray.hh"
#include "CClosure.hh"
#include "CHitInfo.hh"
#include "CObjectHeap.hh"
#include "CScalar.hh"
#include "CModel.hh"
#include "CLight.hh"

#pragma interface

using namespace std;

struct gml_string_hash :public unary_function<const string&,bool> {
    inline size_t operator () (const string& str) const
    {
	string::const_iterator i, e = str.end ();
	unsigned long h = 0;
	for (i = str.begin (); i != e; i++)
	    h = 5*h + *i;
	return size_t(h);
    }
};

class CGml {
public:
    typedef deque<CObjectRef> stack_type;
    //typedef map<string,CObjectRef> mapper_type;

    typedef __gnu_cxx::hash_map<std::string, CObjectRef, gml_string_hash,
	equal_to<std::string> > mapper_type;

protected:
    CObjectHeap	    heap;
    stack_type	    stack;
    mapper_type	    mapper;
    int		    function_nesting;

    CScalarRef  yylex (void);

public:
    CGml ();
    ~CGml ();

    // Allocate cells on the heap.
    CScalarRef valloc (CScalar::value_type _vtype, const char *str = NULL,
	int len = -1);
    CScalarRef valloc (double dbl);
    CScalarRef valloc (long l);
    CScalarRef valloc (const std::string& s);
    CScalarRef valloc (bool b);
    CObjectRef valloc (CObject *o);

    inline long stack_size () const
    {
	return stack.size ();
    }

    // Manipulate the stack.
    template<class T>
    TObjectRef<T>& pop (TObjectRef<T>& ref, const char *op = NULL);

    void push (CObject *obj);
    CArrayRef& pop (CArrayRef& arr, int n);

    // Manipulate the environment.
    CObjectRef lookup (const std::string& name);
    void store (const std::string& name, CObject *obj);
    void store (const std::string& name, CScalar::internal_func_type func);

    void eval_token (CObject *obj);
    void eval_closure (CObject *obj);
    void parse ();
    void dump ();

protected:
    // Used internally by the parser.
    void internal_pop_array (CArrayRef& arr);
    void internal_pop_closure (CClosureRef& clos);
    void internal_pop (CObjectRef& obj, const char *op = NULL);
};

template<class T>
TObjectRef<T>& CGml::pop (TObjectRef<T>& ref, const char *op)
{
    CObjectRef obj;
    internal_pop (obj, op);
    ref = dynamic_cast<T*> ((CObject*)obj);
    if ((CObject*)ref == NULL) {
	cerr <<"ehgml: ";
	if (op != NULL)
	    cerr <<op <<": ";
	cerr <<"wanted " <<T::type_name <<", but got '"
	    <<*obj <<"'." <<endl;
	push (obj);
	throw CGmlError (string("wanted ") + T::type_name + ", but got '"
	    + obj->display_text () + "'", "pop");
    }
    return ref;
}

#endif // ndef CGml_hh

// vi:set sts=4 sw=4:
