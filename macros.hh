#ifndef macros_hh
#define macros_hh

// These macros allow us to use g++'s `return' extension for naming return
// values (and therefore saving constructor/destructor calls) and still be
// portable to compilers which don't have that extension.

#ifdef HAVE_NAMED_RETURN_VALUES

#define NAMED_RETURN return retval
#define DECLARE_RETURN_VALUE(type)
#define RETURN_DEFAULT return

#else // def HAVE_NAMED_RETURN_VALUES

#define NAMED_RETURN
#define DECLARE_RETURN_VALUE(type) type retval;
#define RETURN_DEFAULT return retval

#endif // def HAVE_NAMED_RETURN_VALUES

#endif // macros_hh
