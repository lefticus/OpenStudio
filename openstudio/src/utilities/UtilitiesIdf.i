#ifndef UTILITIES_UTILITIESIDF_I
#define UTILITIES_UTILITIESIDF_I

#ifdef SWIGPYTHON
%module openstudioutilitiesidf
#endif


#define UTILITIES_API
#define UTILITIES_TEMPLATE_EXT

%{
  namespace openstudio{}
  using namespace openstudio;

  #include <utilities/bcl/BCL.hpp>
  #include <utilities/bcl/LocalBCL.hpp>
  #include <utilities/bcl/RemoteBCL.hpp>
  #include <utilities/core/Application.hpp>
  #include <utilities/idf/IdfObject.hpp>
  #include <utilities/idf/WorkspaceObject.hpp>
  #include <QComboBox>
  #include <QMainWindow>
  #include <QNetworkAccessManager>
  #include <utilities/core/PathWatcher.hpp>
  #include <utilities/core/UpdateManager.hpp> 
%}

%include <utilities/core/CommonInclude.i>
%import <utilities/core/CommonImport.i>
%import <utilities/CoreUtilities.i>
%import <utilities/UtilitiesIdd.i>

%include <utilities/idf/Idf.i>

#endif // UTILITIES_UTILITIESIDF_I

