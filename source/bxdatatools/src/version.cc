// version.cc - Implementation of datatools version struct
//
// Copyright (c) 2013 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2013 by The University of Warwick
//
// This file is part of datatools.
//
// datatools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// datatools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with datatools.  If not, see <http://www.gnu.org/licenses/>.

// Ourselves:
#include "datatools/version.h"

// Standard Library:
#include <sstream>

namespace datatools {
int version::get_major() {
  return static_cast<int>(DATATOOLS_VERSION_MAJOR);
}

int version::get_minor() {
  return static_cast<int>(DATATOOLS_VERSION_MINOR);
}

int version::get_patch() {
  return static_cast<int>(DATATOOLS_VERSION_PATCH);
}

std::string version::get_version() {
  static std::string version("");

  if (version.empty()) {
    std::ostringstream stream;
    stream << DATATOOLS_VERSION_MAJOR << "."
           << DATATOOLS_VERSION_MINOR << "."
           << DATATOOLS_VERSION_PATCH;
    version = stream.str();
  }

  return version;
}

bool version::is_at_least(int major, int minor, int patch) {
  if (DATATOOLS_VERSION_MAJOR < major) return false;
  if (DATATOOLS_VERSION_MAJOR > major) return true;
  if (DATATOOLS_VERSION_MINOR < minor) return false;
  if (DATATOOLS_VERSION_MINOR > minor) return true;
  if (DATATOOLS_VERSION_PATCH < patch) return false;
  return true;
}

bool version::has_feature(const std::string&) {
  /// - If you want to add features, then the following implementation
  ///   provides one example based on string features cached in a set.
  ///
  /// static std::set<std::string> features;
  ///
  /// if (features.empty())
  /// {
  ///   // cache the feature list
  ///   features.insert("FASTAPI");
  ///   features.insert("THREADSAFE");
  /// }
  ///
  /// return features.find(name) != features.end();

  return false;
}
} // namespace datatools
