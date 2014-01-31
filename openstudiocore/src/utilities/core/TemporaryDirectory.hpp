#ifndef OPENSTUDIO_TEMPORARYDIRECTORY_
#define OPENSTUDIO_TEMPORARYDIRECTORY_

#include <utilities/CoreUtilitiesAPI.hpp>
#include "Path.hpp"
#include "Logger.hpp"

namespace openstudio
{
  class COREUTILITIES_API TemporaryDirectory
  {
    public:
      TemporaryDirectory();
      ~TemporaryDirectory();


      openstudio::path path();

    private:
      openstudio::path m_path;

      REGISTER_LOGGER("openstudio.utilities.TemporaryDirectory");
  };
}

#endif
