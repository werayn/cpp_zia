//
// EPITECH PROJECT, 2018
// ModuleManager
// File description:
// juniqu_v
//

#include "config_manager/ConfigManager.hpp"

namespace zia::api {

void	ConfigManager::add_default(std::string const &key, std::string const &value)
{
	ConfValue	tmp;

	tmp.v = value;
	_conf[key] = tmp;
	return ;
}

void	ConfigManager::add_default(std::string const &key, long long const &value)
{
	ConfValue	tmp;

	tmp.v = value;
	_conf[key] = tmp;
	return ;
}

void		ConfigManager::DefaultValue()
{
	// here if u want set other default value
	add_default("port", 80);
	add_default("port_ssl", 443);
	add_default("server_name", "Zia");
	add_default("server_version", "1.0");
	add_default("max_uri_size", 1000);
	return ;
}

//Cannonique Forme
ConfigManager::ConfigManager(const std::string &path)
{
	_path = path;
	return ;
}

ConfigManager::ConfigManager(ConfigManager const &p)
{
	_path = p.getPath();
	return ;
}

ConfigManager::~ConfigManager()
{
	return ;
}

ConfigManager	&ConfigManager::operator=(ConfigManager const &p)
{
	_conf = _conf;
	_modules = p.getModules();
	_modules_path = p.getModules_path();
	return (*this);
}

// Getter
zia::api::ModulePathList	ConfigManager::getListModules() const
{
	return (_list);
}

std::string					ConfigManager::getPath() const
{
	return (_path);
}

Conf						&ConfigManager::getConf()
{
	return (_conf);
}

std::vector<std::string>	ConfigManager::getModules() const
{
	return (_modules);
}

std::vector<std::string>	ConfigManager::getModules_path() const
{
	return (_modules_path);
}

std::map<std::string, std::string> 		ConfigManager::getRoots() {
	std::map<std::string, std::string>	roots;

	try {
		if (_conf.find("roots") == _conf.end())
			return roots;
		ConfObject obj = std::get<ConfObject>(_conf["roots"].v);
		for (auto it = obj.begin(); it != obj.end(); it++)
			roots[it->first] = std::get<std::string>(it->second.v);
	} catch (std::exception &) {
		zia::Logger::getInstance().error("ConfigManager: Invalid roots config");
	}
	return roots;
}

JSONValue*					ConfigManager::getDoc() const
{
	return (_doc);
}
// Setter

void		ConfigManager::setModules(std::vector<std::string>const &modules)
{
	for (unsigned int i = 0; i < modules.size(); i++)
		_modules.push_back(modules[i]);
}

void		ConfigManager::setModules_path(std::vector<std::string>const &modules_path)
{
	for (unsigned int i = 0; i < modules_path.size(); i++)
		_modules_path.push_back(modules_path[i]);
}

void		ConfigManager::setPath(std::string const &path)
{
	_path = path;
	return ;
}

void		ConfigManager::setDoc(JSONValue* const &doc)
{
	_doc = doc;
	return ;
}

}
