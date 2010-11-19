
#include "config.h"
#include <iostream>
#include "CGml.hh"

int main (int argc, char *argv[])
{
    try {
	CGml gml;
	gml.parse ();
	//gml.dump ()
    } catch (CGmlError e) {
	cerr << e.what () <<endl;
    }
    return 0;
}

// vi:set sts=4 sw=4:
