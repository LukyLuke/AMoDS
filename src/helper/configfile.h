/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  Lukas Zurschmiede <l.zurschmiede@delightsoftware.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <typeinfo>

namespace helper {
  class Convert
  {
  public:
    template <typename T>
    static std::string T_to_string(T const &value) {
      std::ostringstream ostr;
      ostr << value;
      return ostr.str();
    }
    
    template <typename T>
    static T string_to_T(std::string const &value) {
      std::istringstream istr(value);
      T retval;
      if (!(istr << retval))
        std::cerr << "Config: '" << value << "' is not a valid " << (std::string)typeid(T).name() << std::endl;
      return retval;
    }
  };

  class ConfigFile
  {
  protected:
    std::map<std::string, std::string> values;
    std::string config_file;
    
    bool emptyLine(const std::string &line) const { return line.empty() || (line.find_first_not_of(' ') == line.npos); };
    void removeComment(std::string &line) const;
    bool validLine(const std::string &line) const;
    void extractKey(std::string &key, size_t const &sep_pos, const std::string &line) const;
    void extractValue(std::string &value, size_t const &sep_pos, const std::string &line) const;
    void parseLine(const std::string &line);
    void parseFile();
    void trim(std::string &str) const;

  public:
    ConfigFile(std::string const &file);
    virtual ~ConfigFile();
    bool keyExists(const std::string &key) const { return (values.find(key) != values.end()); };
    
    template <typename ValueType>
    ValueType get(const std::string &key, ValueType const &defaultValue = ValueType()) const {
      return getValue<ValueType>(key, defaultValue);
    }

    template <typename ValueType>
    ValueType getValue(const std::string &key, ValueType const &defaultValue = ValueType()) const {
      return ( !keyExists(key)
        ? defaultValue
        : Convert::string_to_T<ValueType>(values.find(key)->second)
      );
    }
  };

}
#endif // CONFIGFILE_H
