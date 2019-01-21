#include "module_manager/ModuleLoader.hpp"

namespace zia::api {
  ModuleLoader::ModuleLoader() {}

  ModuleLoader::ModuleLoader(ModuleLoader const &copy) {
    _modules = copy.getModules();
  }

  ModuleLoader  &ModuleLoader::operator=(ModuleLoader const &copy) {
    _modules = copy.getModules();
    return (*this);
  }

  ModuleLoader::~ModuleLoader() {
    for (ModulesList::iterator it = _modules.begin(); it < _modules.end(); it++) {
      delete it->module;
      dlclose(it->plib);
    }
  }

  Module      *ModuleLoader::loadModule(std::string const &path, std::string const &name) {
    void      *ptr;
    void      *plib;
    myModule  fptr;
    Module    *module;
    std::string tmp = path + '/' + name;

    if (name.length() < 4 || name.substr(name.length() - 3, 3).compare(".so") != 0)
      throw ModuleLoaderError("expected \".so\" file, got \"" + name + "\"");
    if (!(plib = dlopen(tmp.c_str(), RTLD_LAZY)) || !(ptr = dlsym(plib, "create")))
      throw ModuleLoaderError("invalid library " + name);
    fptr = (myModule)ptr;
    module = (*fptr)();
    unsigned int priority = 100;
    IModule *imodule;
    if ((imodule = dynamic_cast<IModule*>(module)))
      priority = imodule->getPriority();
    ListItem item = {module, priority, plib, name};
    _modules.push_back(item);
    return module;
  }

  Module      *ModuleLoader::reloadModuleByName(std::string const &path, std::string const &name) {
    for (ModulesList::iterator it = _modules.begin(); it < _modules.end(); it++) {
      if (it->name.compare(name) == 0) {
        delete it->module;
        dlclose(it->plib);
        _modules.erase(it);
        return (loadModule(path, name));
      }
    }
    throw ModuleNotFoundError(name);
  }

  void      ModuleLoader::closeModuleByName(std::string const &name) {
    for (ModulesList::iterator it = _modules.begin(); it < _modules.end(); it++) {
      if (it->name.compare(name) == 0) {
        delete it->module;
        dlclose(it->plib);
        _modules.erase(it);
      }
    }
    throw ModuleNotFoundError(name);
  }

  ModulesList  ModuleLoader::getModules() const {
    return _modules;
  }

  Module    *ModuleLoader::getModuleByName(std::string const &name) {
    for (ModulesList::iterator it = _modules.begin(); it < _modules.end(); it++) {
      if (it->name.compare(name) == 0)
        return (it->module);
    }
    throw ModuleNotFoundError(name);
  }
}
