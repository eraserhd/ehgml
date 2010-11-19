#ifndef CObjectHeap_hh
#define CObjectHeap_hh

#pragma interface

#include <deque>
#include "TObjectRef.hh"
#include "CObject.hh"

//////////////////////////////////////////////////////////////////////////////
// CObjectHeap
class CObjectHeap {
protected:
    class Slot {
    public:
	CObject *	obj;
	long		next;	// Next in null or non-null list.
    };

    long	    max_size;

    Slot *	    data;
    long	    null_head;
    long	    nonnull_head;

protected:
    void internal_add (CObject *ptr);

public:
    CObjectHeap (long _max_size = 8192);
    ~CObjectHeap ();

    template<class T>
    TObjectRef<T> add (T *ptr)
    {
	TObjectRef<T> ref (ptr);
	internal_add (ptr);
	return ref;
    }

    void gc ();

};

#endif // ndef CObjectHeap_hh

// vi:set sts=4 sw=4:
