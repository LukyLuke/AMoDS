/*
 *    <one line to give the program's name and a brief idea of what it does.>
 *    Copyright (C) 2011  Lukas Zurschmiede <l.zurschmiede@delightsoftware.com>
 * 
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 * 
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 * 
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <string>
#include <dlfcn.h>
#include <stdio.h>
#include <unistd.h>

#include "module.h"

namespace amods {
      
  Module::Module(const std::string &fileName) {
    static unsigned int BUF_SIZE = 1024;
    char name[BUF_SIZE];
    const char *error;
    file = fileName;
    loaded = false;
    ref_counter = 0;
    
    // TODO: Is there an other way to convert to "char[]" from string or "const *char[]"
    sprintf(name, "%s", std::string("./" + fileName).c_str());
    std::cout << "Load: " << file << std::endl;
    
    // Open the Shared Library
    handle = dlopen(name, RTLD_NOW);
    if (handle == NULL) {
      std::cerr << "Error: " << dlerror() << std::endl;
      return;
    }
    loaded = true;
    
    // Bind EntryFunction "getEngineVersion"
    dlerror();
    f_fnGetEngineVersion = reinterpret_cast<fnGetEngineVersion *>(dlsym(handle, "getEngineVersion"));
    if ((error = dlerror())) {
      std::cerr << "EntryFunction not Found: " << error << std::endl;
      loaded = false;
    }
    
    // Bind EntryFunction "registerPlugin"
    dlerror();
    f_fnRegisterPlugin = reinterpret_cast<fnRegisterPlugin *>(dlsym(handle, "registerPlugin"));
    if ((error = dlerror())) {
      std::cerr << "EntryFunction not Found" << error << std::endl;
      loaded = false;
    }
    
    ref_counter++;
  }
  
  void Module::FreeModule() {
    ref_counter--;
  }
  
  Module::~Module() {
    if ( (handle != NULL) && (ref_counter <= 0) ) {
      const char *error;
      std::cout << "Free: " << file << std::endl;
      dlerror();
      dlclose(handle);
      if ((error = dlerror())) {
        std::cerr << "Error: " << error << std::endl;
      }
      handle = NULL;
    }
  }
  
  
  
}
