#include "ProcessEventsProgressBar.hpp"
#include <utilities/core/Application.hpp>

namespace openstudio {

  ProcessEventsProgressBar::ProcessEventsProgressBar()
    : openstudio::ProgressBar(false, NULL)
  {
  }

  void ProcessEventsProgressBar::onPercentageUpdated(double percentage)
  {
    openstudio::Application::instance().processEvents();
  }

}
