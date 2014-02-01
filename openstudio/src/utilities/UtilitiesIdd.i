#ifndef UTILITIES_UTILITIESIDD_I
#define UTILITIES_UTILITIESIDD_I

#ifdef SWIGPYTHON
%module openstudioutilitiesidd
#endif


#define UTILITIES_API
#define UTILITIES_TEMPLATE_EXT

%include <utilities/core/CommonInclude.i>
%import <utilities/core/CommonImport.i>
%import <utilities/CoreUtilities.i>

%include <utilities/idd/Idd.i>

#endif

