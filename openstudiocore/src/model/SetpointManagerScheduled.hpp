/**********************************************************************
 *  Copyright (c) 2008-2014, Alliance for Sustainable Energy.
 *  All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 **********************************************************************/

#ifndef MODEL_SETPOINTMANAGERSCHEDULED_HPP
#define MODEL_SETPOINTMANAGERSCHEDULED_HPP

#include <model/ModelAPI.hpp>
#include <model/HVACComponent.hpp>

namespace openstudio {

namespace model {

class Schedule;

namespace detail {

class SetpointManagerScheduled_Impl;

}

/** SetpointManagerScheduled is an interface to theIDD object
 *  named "OS:SetpointManager:Scheduled"
 *
 *  The purpose of this class is to simplify the construction and manipulation
 *  of the EnergyPlus SetpointManager:Scheduled object.
 */
class MODEL_API SetpointManagerScheduled : public HVACComponent {
 public:
  /** @name Constructors and Destructors */
  //@{

  /** Constructs a new SetpointManagerScheduled object and places it inside
   *  model. Sets controlVariable to 'Temperature'. Sets setpointSchedule's
   *  ScheduleTypeLimits if necessary and possible. */
  SetpointManagerScheduled(const Model& model, Schedule& setpointSchedule);

  /** Constructs a new SetpointManagerScheduled object and places it inside
   *  model. */
  SetpointManagerScheduled(const Model& model,
                           const std::string& controlVariable,
                           Schedule& setpointSchedule);

  virtual ~SetpointManagerScheduled() {}

  //@}

  static IddObjectType iddObjectType();

  static std::vector<std::string> controlVariableValues();

  /** @name Getters */
  //@{

  /** Returns the Node referred to by the SetpointNodeName field. **/
  boost::optional<Node> setpointNode() const;

  /** Returns the Control Variable **/
  std::string controlVariable() const;

  /** Returns the Schedule **/
  Schedule schedule() const;

  /** Returns true if this object has a schedule, it is an error if this object does not have a schedule **/
  bool hasSchedule() const;

  //@}
  /** @name Setters */
  //@{

  /** Sets the Control Variable **/
  bool setControlVariable(std::string controlVariable);

  /** Sets the Schedule **/
  bool setSchedule(Schedule& schedule);

  //@}
  /** @name Other */
  //@{

  /** Sets control variable and schedule simultaneously. Use in the case that the
   *  control variable change implies a change in schedule units. */
  bool setControlVariableAndSchedule(const std::string& controlVariable,
                                     Schedule& schedule);

  //@}
 protected:
  friend class Model;
  friend class openstudio::IdfObject;
  friend class openstudio::detail::IdfObject_Impl;
  friend class detail::SetpointManagerScheduled_Impl;

  /// @cond
  typedef detail::SetpointManagerScheduled_Impl ImplType;

  explicit SetpointManagerScheduled(boost::shared_ptr<detail::SetpointManagerScheduled_Impl> impl);

 private:
  REGISTER_LOGGER("openstudio.model.SetpointManagerScheduled");
  /// @endcond
};

} // model

} // openstudio

#endif // MODEL_SETPOINTMANAGERSCHEDULED_HPP

