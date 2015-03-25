/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2011  Lukas Zurschmiede <l.zurschmiede@delightsoftware.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <fstream>

#include "configfile.h"

namespace helper {

	ConfigFile::~ConfigFile() {}

	/**
	* Remove everything behind ';' and '#'
	* @param &line 
	* @access protected
	*/
	void ConfigFile::removeComment(std::string &line) const {
		if (line.find('#') != line.npos) {
			line.erase(line.find('#'));
		}
		if (line.find(';') != line.npos) {
			line.erase(line.find(';'));
		}
	}

	/**
	* Check if a Line is a valid Configline or not
	* @param &line
	* @return bool
	* @access protected
	*/
	bool ConfigFile::validLine(const std::string &line) const {
		std::string tmp = line;
		// not in key=value format
		if (tmp.find('=') == tmp.npos) {
			return false;
		}
		tmp.erase(0, tmp.find_first_not_of("\t ")); // trim in front
		// forgotten key
		if (tmp[0] == '=') {
			return false;
		}
		return true;
	}

	/**
	* Remove any Space and Tab in Front and End from a String
	* @param &str
	* @access protected
	*/
	void ConfigFile::trim(std::string &str) const {
		// If there is no  "last char" we have an empty string
		size_t pos = str.find_last_not_of("\t ");
		if (pos != str.npos) {
			str.erase(pos + 1); // Remove everything behind
			pos = str.find_first_not_of("\t ");
			if (pos != str.npos) {
				str.erase(0, pos); // Remove everything in front
			}
		} else {
			str.erase(str.begin(), str.end()); // Remove everything if we have an empty/"Only Spaces" string
		}
	}

	/**
	* Extract the key
	* @param &key the final key
	* @param &sep_pos Separator Position
	* @param &line The line to extract the key from
	* @access protected
	*/
	void ConfigFile::extractKey(std::string &key, size_t const &sep_pos, const std::string &line) const {
		key = line.substr(0, sep_pos);
		trim(key);
	}

	/**
	* Extract the Value
	* @param &value The final value
	* @param &sep_pos Separator Position
	* @param &line The line to extract the Value from
	* @access protected
	*/
	void ConfigFile::extractValue(std::string &value, size_t const &sep_pos, const std::string &line) const {
		value = line.substr(sep_pos+1);
		trim(value);
	}

	/**
	* Extracts the Key and Value from a Line and inserts them in the map
	* @param &line
	* @access protected
	*/
	void ConfigFile::parseLine(const std::string &line) {
		if (validLine(line)) {
			std::string tmp = line;
			tmp.erase(0, tmp.find_first_not_of("\t "));
			
			size_t sep_pos = tmp.find_first_of('=');
			
			std::string key, value;
			extractKey(key, sep_pos, tmp);
			extractValue(value, sep_pos, tmp);
			
			if (!keyExists(key)) {
				values.insert( std::pair<std::string, std::string>(key, value) );
			}
		}
	}

	/**
	* Parse the Configuration-File
	* @access protected
	*/
	void ConfigFile::parseFile() {
		std::ifstream file;
		file.open(config_file.c_str());
		if (!file) {
			std::cerr << "File not found: " << config_file << std::endl;
			return;
		}
		
		std::string line;
		while (std::getline(file, line)) {
			if (line.empty()) {
				continue;
			}
			
			std::string tmp = line;
			removeComment(tmp);
			if (emptyLine(tmp)) {
				continue;
			}
			parseLine(tmp);
		}
		file.close();
	}

	/**
	* Initialization
	* @param &file Configurationfile
	* @access public
	*/
	ConfigFile::ConfigFile(std::string const &file) {
		config_file = file;
		parseFile();
	}

}
