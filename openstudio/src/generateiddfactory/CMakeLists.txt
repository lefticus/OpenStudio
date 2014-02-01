SET( target_name GenerateIddFactory )

# source files
SET( ${target_name}_SRC
  main.cpp
  GenerateIddFactoryOutFiles.hpp
  GenerateIddFactoryOutFiles.cpp
  GenerateIddFactory.hpp
  GenerateIddFactory.cpp
  IddFileFactoryData.hpp
  IddFileFactoryData.cpp
  ../utilities/idd/IddRegex.hpp
  ../utilities/idd/IddRegex.cpp
)

ADD_EXECUTABLE( ${target_name}
  ${${target_name}_SRC}
)

SET_TARGET_PROPERTIES( ${target_name} PROPERTIES COMPILE_DEFINITIONS OPENSTUDIO_DIRECT_INCLUDE )

SET( depends
  ${Boost_LIBRARIES}
  ${QT_LIBS}
  openstudio_coreutilities
)

TARGET_LINK_LIBRARIES( ${target_name}  ${depends} )
