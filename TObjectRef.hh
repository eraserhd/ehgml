#ifndef TObjectRef_hh
#define TObjectRef_hh

//////////////////////////////////////////////////////////////////////////////
// TObjectRef<> - Referenced pointer template.
template<class T>
class TObjectRef {
public:

    typedef T value_type;
    typedef T * pointer_type;
    typedef T& reference_type;
    typedef const T& const_reference_type;
    typedef T const * const_pointer_type;

protected:
    pointer_type ptr;

public:
    TObjectRef ()
	: ptr (NULL)
    {
    }

    TObjectRef (const TObjectRef<T>& rhs)
	: ptr (rhs.ptr)
    {
	if (ptr != NULL)
	    ptr->ref ();
    }

    TObjectRef (pointer_type _ptr)
	: ptr (_ptr)
    {
	if (ptr != NULL)
	    ptr->ref ();
    }

    ~TObjectRef ()
    {
	if (ptr != NULL)
	    ptr->unref ();
    }

    reference_type operator * ()
    {
	return *ptr;
    }

    const_reference_type operator * () const
    {
	return *ptr;
    }

    pointer_type operator -> ()
    {
	return ptr;
    }

    const_pointer_type operator -> () const
    {
	return ptr;
    }

    operator pointer_type ()
    {
	return ptr;
    }

    operator const_pointer_type () const
    {
	return ptr;
    }

    TObjectRef<T>& operator = (const TObjectRef<T>& rhs)
    {
	pointer_type old_ptr = ptr;
	ptr = rhs.ptr;
	if (ptr != NULL)
	    ptr->ref ();
	if (old_ptr != NULL)
	    old_ptr->unref ();
	return *this;
    }
    
    TObjectRef<T>& operator = (pointer_type rhs)
    {
	pointer_type old_ptr = ptr;
	ptr = rhs;
	if (ptr != NULL)
	    ptr->ref ();
	if (old_ptr != NULL)
	    old_ptr->unref ();
	return *this;
    }

    inline bool operator == (const pointer_type rhs) const
    {
	return ptr == rhs;
    }

    inline bool operator != (const pointer_type rhs) const
    {
	return ! operator == (rhs);
    }

    inline bool operator == (const TObjectRef<T>& rhs) const
    {
	return ptr == rhs.ptr;
    }

    inline bool operator != (const TObjectRef<T>& rhs) const
    {
	return ! operator == (rhs);
    }

};

#endif // TObjectRef_hh

// vi:set sts=4 sw=4:
