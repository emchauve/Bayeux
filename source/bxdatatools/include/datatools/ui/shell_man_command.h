//! \file  datatools/ui/shell_man_command.h
//! \brief Shell man command
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
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
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with datatools. If not, see <http://www.gnu.org/licenses/>.

#ifndef DATATOOLS_UI_SHELL_MAN_COMMAND_H
#define DATATOOLS_UI_SHELL_MAN_COMMAND_H

// This project:
#include <datatools/command_utils.h>
#include <datatools/ui/base_command.h>
#include <datatools/ui/basic_shell.h>

namespace datatools {

  namespace ui {

    //! \brief Shell man command
    struct shell_man_command : public const_target_command<basic_shell>
    {
      shell_man_command(const basic_shell &);
      virtual ~shell_man_command();
      virtual void _init(const datatools::properties & config_);
      virtual void _run(datatools::command::returned_info & cri_, uint32_t flags_ = 0);
    };

  } // namespace ui

} // namespace datatools

#endif // DATATOOLS_UI_SHELL_MAN_COMMAND_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
