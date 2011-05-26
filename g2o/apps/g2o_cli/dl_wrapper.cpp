// g2o - General Graph Optimization
// Copyright (C) 2011 R. Kuemmerle, G. Grisetti, W. Burgard
//
// g2o is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// g2o is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <sys/types.h>

#include <cstdio>
#include <iostream>
#include <algorithm>

#include "dl_wrapper.h"
#include "g2o/stuff/filesys_tools.h"

#if defined (UNIX) || defined(CYGWIN)
#include <dlfcn.h>
#endif

using namespace std;

namespace g2o {

DlWrapper::DlWrapper()
{
}

DlWrapper::~DlWrapper()
{
  clear();
}

int DlWrapper::openLibraries(const std::string& directory, const std::string& pattern)
{
  cerr << "# loading libraries from " << directory << "\t pattern: " << pattern << endl;
  string searchPattern = directory + "/" + pattern;
  if (pattern == "")
    searchPattern = directory + "/*";
  vector<string> matchingFiles = getFilesByPattern(searchPattern.c_str());

  int numLibs = 0;
  for (size_t i = 0; i < matchingFiles.size(); ++i) {
    const string& filename = matchingFiles[i];
    if (find(_filenames.begin(), _filenames.end(), filename) != _filenames.end())
      continue;

    // open the lib
    //cerr << "loading " << filename << endl;
    if (openLibrary(filename))
      numLibs++;
  }

  return numLibs;
}

void DlWrapper::clear()
{
# if defined (UNIX) || defined(CYGWIN)
  for (size_t i = 0; i < _handles.size(); ++i) {
    dlclose(_handles[i]);
  }
#elif defined(WINDOWS)
  for (size_t i = 0; i < _handles.size(); ++i) {
    FreeLibrary(_handles[i]);
  }
#endif
  _filenames.clear();
  _handles.clear();
}

bool DlWrapper::openLibrary(const std::string& filename)
{
# if defined (UNIX) || defined(CYGWIN)
  void* handle = dlopen(filename.c_str(), RTLD_LAZY);
  if (! handle) {
    cerr << "Cannot open library: " << dlerror() << '\n';
    return false;
  }
# elif defined (WINDOWS)
  HMODULE handle = LoadLibrary(filename.c_str());
  if (! handle) {
    cerr << "Cannot open library." << endl;
    return false;
  }
# endif

  //cerr << "loaded " << filename << endl;

  _filenames.push_back(filename);
  _handles.push_back(handle);
  return true;
}

} // end namespace g2o
