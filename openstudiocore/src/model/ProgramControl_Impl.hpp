/**********************************************************************
 *  Copyright (c) 2008-2013, Alliance for Sustainable Energy.
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

#ifndef MODEL_PROGRAMCONTROL_IMPL_HPP
#define MODEL_PROGRAMCONTROL_IMPL_HPP

#include <model/ModelAPI.hpp>
#include <model/ModelObject_Impl.hpp>

namespace openstudio {
namespace model {

namespace detail {

  /** ProgramControl_Impl is a ModelObject_Impl that is the implementation class for ProgramControl.*/
  class MODEL_API ProgramControl_Impl : public ModelObject_Impl {
    Q_OBJECT;

    Q_PROPERTY(boost::optional<int> numberofThreadsAllowed READ numberofThreadsAllowed WRITE setNumberofThreadsAllowed RESET resetNumberofThreadsAllowed);

    // TODO: Add relationships for objects related to this one, but not pointed to by the underlying data.
    //       Such relationships can be generated by the GenerateRelationships.rb script.
   public:
    /** @name Constructors and Destructors */
    //@{

    ProgramControl_Impl(const IdfObject& idfObject,
                        Model_Impl* model,
                        bool keepHandle);

    ProgramControl_Impl(const openstudio::detail::WorkspaceObject_Impl& other,
                        Model_Impl* model,
                        bool keepHandle);

    ProgramControl_Impl(const ProgramControl_Impl& other,
                        Model_Impl* model,
                        bool keepHandle);

    virtual ~ProgramControl_Impl() {}

    //@}

    /** @name Virtual Methods */
    //@{

    virtual const std::vector<std::string>& outputVariableNames() const;

    virtual IddObjectType iddObjectType() const;

    //@}
    /** @name Getters */
    //@{

    boost::optional<int> numberofThreadsAllowed() const;

    //@}
    /** @name Setters */
    //@{

    bool setNumberofThreadsAllowed(boost::optional<int> numberofThreadsAllowed);

    void resetNumberofThreadsAllowed();

    //@}
    /** @name Other */
    //@{

    //@}
   protected:
   private:
    REGISTER_LOGGER("openstudio.model.ProgramControl");
  };

} // detail

} // model
} // openstudio

#endif // MODEL_PROGRAMCONTROL_IMPL_HPP
