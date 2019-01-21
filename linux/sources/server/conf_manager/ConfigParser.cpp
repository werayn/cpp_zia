//
// EPITECH PROJECT, 2018
// ModuleManager
// File description:
// juniqu_v
//

#include "logger.hpp"
#include "config_manager/ConfigManager.hpp"

namespace zia::api {

bool		ConfigManager::browser_conf()
{
	DefaultValue();
	try {
		CheckPath();
	}
	catch (std::exception &err)
	{
	    zia::Logger::getInstance().error(err.what());
		return (false);
	}
	JSONObject mod;
	mod = getDoc()->AsObject();
	find_modules(mod);
	find_modules_path(mod);
	if (_modules.empty() == false || _modules_path.empty() != false)
		getModByPath();
	getKey();
	delete _doc;
	return (true);
}

void	ConfigManager::getKey()
{
	std::vector<std::wstring> keys = _doc->ObjectKeys();
	std::vector<std::wstring>::iterator iter = keys.begin();
	std::string key;
	std::string value;

	while (iter != keys.end())
	{
		key = format_wstring(*iter);
		JSONValue *key_value = _doc->Child((*iter).c_str());
		if (key_value && !key_value->IsNull())
		{
			if (key_value->IsString())
				add_string(key, key_value->Stringify().c_str());
			else if (key_value->IsNumber())
				add_num(key, key_value->AsNumber());
			else if (key_value->IsBool())
				add_bool(key, (key_value->AsBool() ? (true) : (false)));
			else if (key_value->IsArray())
				add_array(key, key_value);
			else if (key_value->IsObject())
				add_object(key, key_value);
		}
		iter++;
	}
}

//Obj adder
void		ConfigManager::add_object(std::string key, JSONValue *key_value)
{
	ConfObject	obj;
	ConfValue 	end;
	std::vector<std::wstring> keys = key_value->ObjectKeys();
	std::vector<std::wstring>::iterator iter = keys.begin();
	std::string tmp;

	while (iter != keys.end())
	{
		tmp = format_wstring(*iter);
		JSONValue *value = key_value->Child((*iter).c_str());
		if (value)
			{
				if (value->IsString())
					obj = add_string_to_obj(tmp, value->Stringify(), obj);
				else if (value->IsBool())
					obj = add_bool_to_obj(tmp, (value->AsBool() ? (true) : (false)), obj);
				else if (value->IsNumber())
					obj =  add_number_to_obj(tmp, value->AsNumber(), obj);
				else if (value->IsArray())
					obj = add_array_for_obj(tmp, value, obj);
				else if (value->IsObject())
					obj = add_obj_to_obj(tmp, value, obj);		
			}
		iter++;
	}
	end.v = obj;
	_conf[key] = end;
}

Conf		ConfigManager::add_obj_to_obj(std::string key, JSONValue *key_value, ConfObject obj_end)
{
	ConfObject	obj;
	ConfValue 	end;
	std::vector<std::wstring> keys = key_value->ObjectKeys();
	std::vector<std::wstring>::iterator iter = keys.begin();
	std::string tmp;

	while (iter != keys.end())
	{
		tmp = format_wstring(*iter);
		JSONValue *value = key_value->Child((*iter).c_str());
		if (value)
			{
				if (value->IsString())
					obj = add_string_to_obj(tmp, value->Stringify(), obj);
				else if (value->IsBool())
					obj = add_bool_to_obj(tmp, (value->AsBool() ? (true) : (false)), obj);
				else if (value->IsNumber())
					obj =  add_number_to_obj(tmp, value->AsNumber(), obj);
				else if (value->IsArray())
					obj = add_array_for_obj(tmp, value, obj);
				else if (value->IsObject())
					obj = add_obj_to_obj(tmp, value, obj);		
			}
		iter++;
	}
	end.v = obj;
	obj_end[key] = end;
	return (obj_end);
}

Conf		ConfigManager::new_object(JSONValue *key_value)
{
	ConfObject	obj;
	ConfValue 	end;
	std::vector<std::wstring> keys = key_value->ObjectKeys();
	std::vector<std::wstring>::iterator iter = keys.begin();
	std::string tmp;

	while (iter != keys.end())
	{
		tmp = format_wstring(*iter);
		JSONValue *value = key_value->Child((*iter).c_str());
		if (value)
			{
				if (value->IsString())
					obj = add_string_to_obj(tmp, value->Stringify(), obj);
				else if (value->IsBool())
					obj = add_bool_to_obj(tmp, (value->AsBool() ? (true) : (false)), obj);
				else if (value->IsNumber())
					obj =  add_number_to_obj(tmp, value->AsNumber(), obj);
				else if (value->IsArray())
					obj = add_array_for_obj(tmp, value, obj);
				else if (value->IsObject())
					obj = add_obj_to_obj(tmp, value, obj);
			}
		iter++;
	}
	return (obj);
}

// ARRAY adder
ConfObject	ConfigManager::add_array_for_obj(std::string key, JSONValue *key_value, ConfObject obj_end)
{
	double			nb;
	bool 			res;
	std::string 	tmp;
	ConfArray		obj;
	ConfValue		value;

	JSONArray array = key_value->AsArray();
	for (unsigned int i = 0; i < array.size(); i++)
	{
		if (array[i]->IsString())
		{
			tmp = format_wstring(array[i]->Stringify());
			tmp.erase(std::remove(tmp.begin(), tmp.end(), '\"' ), tmp.end());
			tmp.erase(std::remove(tmp.begin(), tmp.end(), '\\' ), tmp.end());
			value = parse_str(tmp);
			obj.push_back(value);
		}
		else if (array[i]->IsBool())
		{
			key_value->AsBool() ? (res = true) : (res = false);
			value.v = res;
			obj.push_back(value);
		}
		else if (array[i]->IsNumber())
		{
			nb = array[i]->AsNumber();
			value.v = nb;
			if (isLong(nb) == true)
				value.v = (long long)nb;
			obj.push_back(value);
		}
		else if (array[i]->IsArray())
		{
			value = new_array(array[i]);
			obj.push_back(value);
		}
		else if (array[i]->IsObject())
		{
			value.v = new_object(array[i]);
			obj.push_back(value);
		}
	}
	ConfValue 	end;
	end.v = obj;
	obj_end[key] = end;
	return (obj_end);
}

void	ConfigManager::add_array(std::string key, JSONValue *key_value)
{
	double			nb;
	bool 			res;
	std::string 	tmp;
	ConfArray		obj;
	ConfArray 		tmp_obj;
	ConfValue		value;

	JSONArray array = key_value->AsArray();
	for (unsigned int i = 0; i < array.size(); i++)
	{
		if (array[i]->IsString())
		{
			tmp = format_wstring(array[i]->Stringify());
			tmp.erase(std::remove(tmp.begin(), tmp.end(), '\"' ), tmp.end());
			tmp.erase(std::remove(tmp.begin(), tmp.end(), '\\' ), tmp.end());
			value = parse_str(tmp);
			obj.push_back(value);
		}
		else if (array[i]->IsBool())
		{
			key_value->AsBool() ? (res = true) : (res = false);
			value.v = res;
			obj.push_back(value);
		}
		else if (array[i]->IsNumber())
		{
			nb = array[i]->AsNumber();
			value.v = nb;
			if (isLong(nb) == true)
				value.v = (long long)nb;
			obj.push_back(value);
		}
		else if (array[i]->IsArray())
		{
			value = new_array(array[i]);
			obj.push_back(value);
		}
		else if (array[i]->IsObject())
		{
			value.v = new_object(array[i]);
			obj.push_back(value);
		}
	}
	ConfValue 	end;
	end.v = obj;
	_conf[key] = end;
}

ConfValue	ConfigManager::new_array(JSONValue *arr)
{
	std::string str;
	double		nb;
	bool 		boolean;
	ConfArray 	res;
	ConfValue 	end;
	ConfValue 	tmp;

	JSONArray array = arr->AsArray();
	for (unsigned int i = 0; i < array.size(); i++)
	{
		if (array[i]->IsString())
		{
			str = format_wstring(array[i]->Stringify());
			str.erase(std::remove(str.begin(), str.end(), '\"' ), str.end());
			str.erase(std::remove(str.begin(), str.end(), '\\' ), str.end());
			tmp = parse_str(str);
			res.push_back(tmp);
		}
		else if (array[i]->IsBool())
		{
			arr->AsBool() ? (boolean = true) : (boolean = false);
			tmp.v = boolean;
			res.push_back(tmp);
		}
		else if (array[i]->IsNumber())
		{
			nb = array[i]->AsNumber();
			tmp.v = nb;
			if (isLong(nb) == true)
				tmp.v = (long long)nb;
			res.push_back(tmp);
		}
		else if (array[i]->IsArray())
		{
			tmp = new_array(array[i]);
			res.push_back(tmp);
		}
		else if (array[i]->IsObject())
		{
			tmp.v = new_object(array[i]);
			res.push_back(tmp);
		}
	}
	end.v = res;
	return (end);
}

// Number, Bool and string adder
ConfObject	ConfigManager::add_number_to_obj(std::string key, double num, ConfObject obj)
{
	ConfValue res;

	res.v = num;
	if (isLong(num) == true)
		res.v = (long long)num;
	obj[key] = res;
	return (obj);
}

void	ConfigManager::add_num(std::string key, double num)
{
	ConfValue res;

	res.v = num;
	if (isLong(num) == true)
		res.v = (long long)num;
	_conf[key] = res;
	return ;
}

void	ConfigManager::add_string(std::string key, std::wstring tmp)
{
	std::string 	value;
	ConfValue		val;

	value = format_wstring(tmp);
	value.erase(std::remove(value.begin(), value.end(), '\"' ), value.end());
	value.erase(std::remove(value.begin(), value.end(), '\\' ), value.end());
	val = parse_str(value);
	_conf[key] = val;
}

void	ConfigManager::add_bool(std::string key, bool res)
{
	ConfValue		val;

	val.v = res;
	_conf[key] = val;
}

ConfObject		ConfigManager::add_string_to_obj(std::string key, std::wstring str, ConfObject obj)
{
	ConfValue res;
	std::string 	tmp;

	tmp = format_wstring(str);
	tmp.erase(std::remove(tmp.begin(), tmp.end(), '\"' ), tmp.end());
	tmp.erase(std::remove(tmp.begin(), tmp.end(), '\\' ), tmp.end());
	res = parse_str(tmp);
	obj[key] = res;
	return obj;
}

ConfObject		ConfigManager::add_bool_to_obj(std::string key, bool ret, ConfObject obj)
{
	ConfValue res;

	res.v = ret;
	obj[key] = res;
	return obj;
}

}
