#ifndef MODULELAUNCHER_HPP_
# define MODULELAUNCHER_HPP_

# include <iostream>
# include <memory>
# include <dlfcn.h>
# include "IModule.hpp"
# include "logger.hpp"
# include "api/module.h"
# include "error/Error.hpp"

namespace zia::api {
  typedef Module *(*myModule)();

  struct          ListItem {
    Module        *module;
    unsigned int  priority;
    void          *plib;
    std::string   name;
  };

  using ModulesList = std::vector<ListItem>;

  class     ModuleLoader {
  public:
    ModuleLoader();
    ModuleLoader(ModuleLoader const &);
    ModuleLoader  &operator=(ModuleLoader const &);
    ~ModuleLoader();

    /**
    * Load the module [name] from the location [path]
    * Exception : ModuleLoaderError
    * \return a pointer to the loaded module (automatically deleted at destruction)
    */
    Module        *loadModule(std::string const &path, std::string const &name);

    /**
    * Reload the module [name] from the location [path]
    * Close the older module
    * Exception : ModuleNotFoundError, inherit ModuleLoaderError
    * \return a pointer to the loaded module (automatically deleted at destruction)
    */
    Module        *reloadModuleByName(std::string const &path, std::string const &name);

    /**
    * Close the module [name]
    * Exception : ModuleNotFoundError
    */
    void          closeModuleByName(std::string const &name);

    /**
    * Get a module by its [name]
    * Exception : ModuleNotFoundError
    * \return a pointer to the module
    */
    Module        *getModuleByName(std::string const &name);

    /**
    * Getters
    */
    ModulesList   getModules() const;

  protected:
    ModulesList     _modules;
  };
}

#endif /* end of include guard: MODULELAUNCHER_HPP_ */
