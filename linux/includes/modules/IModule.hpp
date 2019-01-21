#ifndef IMODULE_HPP_
# define IMODULE_HPP_

# include "module.h"

namespace zia::api {
  class   IModule : public Module {
  public:
    /**
    * Call to get lhe level of priority of the module
    * (from 10 (priority) to 100 (not priority)) default : 100
    * before 10 : reserved to core modules like SSL and PHP
    * \return the level of priority
    */
    virtual unsigned int getPriority() const = 0;
  };
} /* zia::api */

#endif /* end of include guard: IMODULE_HPP_ */
