# create a library out of the utilities namespace

SET( target_name openstudio_utilities)

INCLUDE ( idd/CMakeLists.txt )
INCLUDE ( idf/CMakeLists.txt )
INCLUDE ( sql/CMakeLists.txt )



SET( document_src
  document/Document.hpp
  document/Document.cpp
  document/DocumentEnums.hpp
  document/DocumentRegex.hpp
  document/DocumentRegex.cpp
  document/HeadersAndFooters.hpp
  document/HeadersAndFooters.cpp
  document/DocumentElement.hpp
  document/DocumentElement_Impl.hpp
  document/DocumentElement.cpp
  document/SectionHeading.hpp
  document/SectionHeading_Impl.hpp
  document/SectionHeading.cpp
  document/SectionElement.hpp
  document/SectionElement_Impl.hpp
  document/SectionElement.cpp
  document/Section.hpp
  document/Section_Impl.hpp
  document/Section.cpp
  document/Text.hpp
  document/Text_Impl.hpp
  document/Text.cpp
  document/Table.hpp
  document/Table_Impl.hpp
  document/Table.cpp
  document/TableElement.hpp
  document/TableElement_Impl.hpp
  document/TableElement.cpp
)

SET( filetypes_src
  filetypes/EpwFile.hpp
  filetypes/EpwFile.cpp
  filetypes/TimeDependentValuationFile.hpp
  filetypes/TimeDependentValuationFile.cpp
)

SET( geometry_src
  geometry/BoundingBox.hpp
  geometry/BoundingBox.cpp
  geometry/EulerAngles.hpp
  geometry/EulerAngles.cpp  
  geometry/Geometry.hpp
  geometry/Geometry.cpp  
  geometry/Intersection.hpp
  geometry/Intersection.cpp  
  geometry/Plane.hpp
  geometry/Plane.cpp
  geometry/Point3d.hpp
  geometry/Point3d.cpp
  geometry/Transformation.hpp
  geometry/Transformation.cpp
  geometry/Vector3d.hpp
  geometry/Vector3d.cpp
  ../polypartition/polypartition.cpp
)


SET( plot_src
  plot/AnnotatedTimeline.hpp
  plot/AnnotatedTimeline.cpp
  plot/Chart.hpp
  plot/BarChart.hpp
  plot/PieChart.hpp
  plot/FloodPlot.hpp
  plot/FloodPlot.cpp
  plot/LinePlot.hpp
  plot/LinePlot.cpp
  plot/Plot2D.hpp
  plot/Plot2D.cpp
  plot/ProgressBar.hpp
  plot/ProgressBar.cpp
)

SET( cloud_src
  cloud/AWSProvider.hpp
  cloud/AWSProvider_Impl.hpp
  cloud/AWSProvider.cpp
  cloud/CloudProvider.hpp
  cloud/CloudProvider_Impl.hpp
  cloud/CloudProvider.cpp
  cloud/OSServer.hpp
  cloud/OSServer_Impl.hpp
  cloud/OSServer.cpp
  cloud/VagrantProvider.hpp
  cloud/VagrantProvider_Impl.hpp
  cloud/VagrantProvider.cpp
)

SET( documentation_src
  mainpage.hpp
)

SET( economics_src
  economics/CostLineItem.hpp
  economics/CostLineItem.cpp
  economics/CostObject.hpp
  economics/CostObject.cpp
  economics/Economics.hpp
  economics/Economics.cpp
)

SET( ${target_name}_depends
  openstudio_coreutilities
  boost_log
  zkqwt
  sqlite
  litesql
  litesql-util
  miniziplib
  qjson
  ${Boost_LIBRARIES}
  ${CMAKE_THREAD_LIBS}
  ${QT_LIBS}
)

IF(WIN32)
  LIST(APPEND ${target_name}_depends qtwinmigrate)
ENDIF(WIN32)

# moc files
SET( bcl_moc
  bcl/BCL.hpp
  bcl/LocalBCL.hpp
  bcl/RemoteBCL.hpp
)

SET( cloud_moc
  cloud/AWSProvider_Impl.hpp
  cloud/CloudProvider_Impl.hpp
  cloud/OSServer_Impl.hpp
  cloud/VagrantProvider_Impl.hpp
)

SET( core_moc
  core/PathWatcher.hpp
  core/UpdateManager.hpp
)

SET( idf_moc
  idf/IdfObject_Impl.hpp
  idf/IdfObjectWatcher.hpp
  idf/WorkspaceObjectWatcher.hpp
  idf/WorkspaceObject_Impl.hpp
  idf/Workspace_Impl.hpp
  idf/WorkspaceWatcher.hpp
)

SET( data_moc
  data/Attribute_Impl.hpp
)

SET( plot_moc
  plot/Plot2D.hpp
  plot/FloodPlot.hpp
  plot/LinePlot.hpp
  plot/ProgressBar.hpp
)

###############################################################################
# idf_moc files include generated source code.  It is important that these
# generated files are generated before moc runs.
# The following commands make sure that happens.
# This could be done by simply touching the moc'ed files in the source tree
# but in the spirit of keeping the source tree clean, we copy them into the 
# build directory instead.
###############################################################################
FOREACH( f IN LISTS idf_moc )
  STRING( REGEX REPLACE hpp$ hxx fxx ${f} )
  LIST( APPEND idfxx_moc ${CMAKE_CURRENT_BINARY_DIR}/${fxx} )
  ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${fxx}
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${f} ${CMAKE_CURRENT_BINARY_DIR}/${fxx} 
    DEPENDS
    ${CMAKE_CURRENT_BINARY_DIR}/idd/IddEnums.hxx
    ${CMAKE_CURRENT_BINARY_DIR}/idd/IddFieldEnums.hxx
    ${CMAKE_CURRENT_BINARY_DIR}/idd/IddFactory.hxx
    ${CMAKE_CURRENT_BINARY_DIR}/idd/IddFactory.cxx 
    ${CMAKE_CURRENT_SOURCE_DIR}/${f}
  )
ENDFOREACH()
###############################################################################

## Qt MOC generation
qt4_wrap_cpp( cloud_moc_src ${cloud_moc} )
qt4_wrap_cpp( idf_moc_src ${idfxx_moc} )
qt4_wrap_cpp( plot_moc_src ${plot_moc} )

# set up groups of source files for Visual Studio
SOURCE_GROUP( cloud FILES ${cloud_src} )
SOURCE_GROUP( document FILES ${document_src} )
SOURCE_GROUP( filetypes FILES ${filetypes_src} )
SOURCE_GROUP( idd FILES ${idd_src} )
SOURCE_GROUP( idf FILES ${idf_src} )
SOURCE_GROUP( sql FILES ${sql_src} )
SOURCE_GROUP( geometry FILES ${geometry_src} )
SOURCE_GROUP( plot FILES ${plot_src} )
SOURCE_GROUP( documentation FILES ${documentation_src} )
SOURCE_GROUP( economics FILES ${economics_src} )


ADD_CUSTOM_COMMAND(
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/SWIGRubyRuntime.hxx"
  COMMAND "${SWIG_EXECUTABLE}"
          "-ruby" 
          -external-runtime "${CMAKE_CURRENT_BINARY_DIR}/SWIGRubyRuntime.hxx"
)


SET( ${target_name}_src
  UtilitiesAPI.hpp
  "${CMAKE_CURRENT_BINARY_DIR}/SWIGRubyRuntime.hxx" # referenced here to make sure it is generated when and where we expect
  ${cloud_src}
  ${cloud_moc_src}
  ${document_src}
  ${documentation_src} 
  ${economics_src}
  ${filetypes_src} 
  ${idd_src}
  ${idf_src}
  ${idf_moc_src}
  ${sql_src}
  ${geometry_src}
  ${plot_src}
  ${plot_moc_src}
)

SET( ${target_name}_test_src
  ${idd_test_src}
  ${idf_test_src}
  ${idf_test_moc_src}
  ${sql_test_src}
  cloud/test/AWSProvider_GTest.cpp
  cloud/test/VagrantProvider_GTest.cpp
  document/test/DocumentFixture.hpp
  document/test/DocumentFixture.cpp
  document/test/Document_GTest.cpp
  document/test/DocumentRegex_GTest.cpp
  document/test/SectionHeading_GTest.cpp
  document/test/Table_GTest.cpp
  document/test/TableElement_GTest.cpp  
  document/test/Text_GTest.cpp
  economics/test/Economics_GTest.cpp
  filetypes/test/EpwFile_GTest.cpp
  filetypes/test/TimeDependentValuationFile_GTest.cpp
  geometry/Test/BoundingBox_GTest.cpp
  geometry/Test/GeometryFixture.hpp
  geometry/Test/GeometryFixture.cpp
  geometry/Test/Geometry_GTest.cpp
  geometry/Test/Intersection_GTest.cpp
  geometry/Test/Plane_GTest.cpp
  geometry/Test/Transformation_GTest.cpp
  plot/Test/AnnotatedTimeline_GTest.cpp
  plot/Test/BarChart_GTest.cpp
  plot/Test/FloodPlot_GTest.cpp
  plot/Test/LinePlot_GTest.cpp
  plot/Test/PieChart_GTest.cpp
  plot/Test/ProgressBar_GTest.cpp
)

SET( ${target_name}_swig_src
  #  Utilities.i
  ${CMAKE_BINARY_DIR}/src/OpenStudio.hxx
  ${idd_swig_src}
  ${idf_swig_src}
  ${sql_swig_src}
  cloud/Cloud.i
  economics/Economics.i
  filetypes/Filetypes.i
  geometry/Geometry.i
  document/Document.i
  plot/Plot.i
  plot/AnnotatedTimeline.i
  plot/FloodPlot.i
  plot/LinePlot.i
  plot/Plot2D.i
  plot/ProgressBar.i
)


#include subdir for pch
add_subdirectory(pch)

ADD_LIBRARY( ${target_name} SHARED
  ${${target_name}_src}
)

AddPCH(${target_name})

IF( NOT APPLE )
INSTALL(TARGETS ${target_name}
  RUNTIME DESTINATION bin 
  LIBRARY DESTINATION lib )
ENDIF()

TARGET_LINK_LIBRARIES(${target_name} ${${target_name}_depends})
CREATE_SRC_GROUPS( "${${target_name}_test_src}" )
CREATE_TEST_TARGETS( ${target_name} "${${target_name}_test_src}" "${${target_name}_depends}"  )

IF( BUILD_TESTING )
  ADD_DEPENDENCIES("${target_name}_tests" openstudio_energyplus_resources openstudio_standardsinterface_resources)
ENDIF()

CREATE_SRC_GROUPS( "${${target_name}_swig_src}" )


MAKE_SWIG_TARGET( OpenStudioUtilitiesIdd utilitiesidd "${CMAKE_CURRENT_SOURCE_DIR}/UtilitiesIdd.i" "${${target_name}_swig_src}" ${target_name} "" )

MAKE_SWIG_TARGET( OpenStudioUtilitiesIdf utilitiesidf "${CMAKE_CURRENT_SOURCE_DIR}/UtilitiesIdf.i" "${${target_name}_swig_src}" ${target_name} OpenStudioUtilitiesIdd )

MAKE_SWIG_TARGET( OpenStudioUtilitiesCloud utilitiescloud "${CMAKE_CURRENT_SOURCE_DIR}/UtilitiesCloud.i" "${${target_name}_swig_src}" ${target_name} "")
MAKE_SWIG_TARGET( OpenStudioUtilitiesDocument utilitiesdocument "${CMAKE_CURRENT_SOURCE_DIR}/UtilitiesDocument.i" "${${target_name}_swig_src}" ${target_name} OpenStudioUtilitiesCloud)
MAKE_SWIG_TARGET( OpenStudioUtilitiesEconomics utilitieseconomics "${CMAKE_CURRENT_SOURCE_DIR}/UtilitiesEconomics.i" "${${target_name}_swig_src}" ${target_name} OpenStudioUtilitiesDocument )
MAKE_SWIG_TARGET( OpenStudioUtilitiesGeometry utilitiesgeometry "${CMAKE_CURRENT_SOURCE_DIR}/UtilitiesGeometry.i" "${${target_name}_swig_src}" ${target_name} OpenStudioUtilitiesEconomics )
MAKE_SWIG_TARGET( OpenStudioUtilitiesPlot utilitiesplot "${CMAKE_CURRENT_SOURCE_DIR}/UtilitiesPlot.i" "${${target_name}_swig_src}" ${target_name} OpenStudioUtilitiesGeometry )
MAKE_SWIG_TARGET( OpenStudioUtilitiesSql utilitiessql "${CMAKE_CURRENT_SOURCE_DIR}/UtilitiesSql.i" "${${target_name}_swig_src}" ${target_name} OpenStudioUtilitiesPlot )
MAKE_SWIG_TARGET( OpenStudioUtilitiesFileTypes utilitiesfiletypes "${CMAKE_CURRENT_SOURCE_DIR}/UtilitiesFileTypes.i" "${${target_name}_swig_src}" ${target_name} OpenStudioUtilitiesSql)

MAKE_SWIG_TARGET( OpenStudioUtilities utilities "${CMAKE_CURRENT_SOURCE_DIR}/Utilities.i" "${${target_name}_swig_src}" ${target_name} OpenStudioUtilitiesFileTypes )



