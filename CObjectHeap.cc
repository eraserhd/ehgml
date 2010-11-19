
#pragma implementation "CObjectHeap.hh"

#include "config.h"
#include <cstddef>
#include <cassert>
#include <algorithm>
#include "CObject.hh"
#include "CObjectHeap.hh"
#include "CGmlError.hh"

//////////////////////////////////////////////////////////////////////////////
// CObjectHeap

CObjectHeap::CObjectHeap (long _max_size)
    : max_size (_max_size), null_head (0), nonnull_head (-1)
{
    // Initialize arena.
    data = new Slot[_max_size];
    for (long i = 0; i < _max_size; i++) {
	data[i].obj = NULL;
	data[i].next = (i+1 == _max_size) ? -1 : i+1;
    }
}

CObjectHeap::~CObjectHeap ()
{
    long i = nonnull_head;
    while (i != -1) {
	delete data[i].obj;
	i = data[i].next;
    }

    delete[] data;
}

void CObjectHeap::internal_add (CObject *ptr)
{
    if (null_head == -1) {
	gc ();
	if (null_head == -1)
	    throw CGmlError ("no empty cells", "heap");
    }

    // Remove cell from the head of the null list.
    long cell = null_head;
    assert (cell != -1);
    null_head = data[cell].next;

    // Add cell to the head of the nonnull list.
    data[cell].next = nonnull_head;
    nonnull_head = cell;

    // Set the contents of the cell.
    data[cell].obj = ptr;
}

void CObjectHeap::gc ()
{
    long i, next, prev;
    CObject *obj;

    // Mark all referenced cells.
    i = nonnull_head;
    while (i != -1) {
	obj = data[i].obj;
	if (! obj->marked () && obj->referenced () )
	    obj->mark ();
	i = data[i].next;
    }

    i = nonnull_head;
    prev = -1;
    while (i != -1) {
	next = data[i].next;
	obj = data[i].obj;	

	if (!obj->marked ()) {
	    delete obj;

	    // Remove cell from nonnull list.
	    if (prev == -1)
		nonnull_head = next;
	    else
		data[prev].next = next;

	    // Insert cell on null list.
	    data[i].next = null_head;
	    null_head = i;
	} else {
	    obj->unmark ();
	    prev = i; // Prev is really the previous nonnull cell.
	}

	i = next;
    }
}

// vi:set sts=4 sw=4:
