// datatools/ui/shell_tree_command.cc
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

// Ourselves
#include <datatools/ui/shell_tree_command.h>

// This project:
#include <datatools/ui/utils.h>
#include <datatools/ui/traits.h>
#include <datatools/ui/ansi_colors.h>
#include <datatools/detail/command_macros.h>

namespace datatools {

  namespace ui {

    shell_tree_command::shell_tree_command(const basic_shell & shell_)
      : datatools::ui::const_target_command<basic_shell>(shell_, "tree", "List interfaces and commands")
    {
      return;
    }

    shell_tree_command::~shell_tree_command()
    {
      if (is_initialized()) {
        reset();
      }
      return;
    }

    void shell_tree_command::_init(const datatools::properties & config_)
    {
      this->base_command::_init(config_);

      // Arguments description:
      _grab_opts().add_options()

        ("path,p",
         boost::program_options::value<std::string>()
         ->value_name("path"),
         "Path to be listed\n"
         "Examples: \n"
         "  tree --path / \n"
         "  tree ~ \n"
         "  tree ./ \n"
         "  tree /"
         )

        ("all,a",
         "Do not ignore entries starting with ."
         )

        ("long,l",
         "Use a long listing format"
         )

        ("color",
         "Use colored output"
         )

        ("classify,F",
         "Append indicator (one of /*) to entries"
         )

        ("dirsonly,d",
         "List directories only"
         )

        ("prune",
         "Prune empty directories"
         )

        ("full-path,f",
         "Prints the full path prefix for each entry"
         )

        ("dirsfirst",
         "List directories before files"
         )

        ("level,L",
         boost::program_options::value<uint32_t>()
         ->value_name("level")
         ->default_value(0),
         "Max display depth of the directory tree"
         )

        ; // end of options description

      // Positional options:
      _grab_args().add("path", 1);

      return;
    }

    void shell_tree_command::_run(datatools::command::returned_info & cri_, uint32_t /*flags_*/)
    {
      cri_.reset();
      cri_.set_error_code(datatools::command::CEC_SUCCESS);
      DT_LOG_TRACE(get_logging_priority(), "Running command '" << get_name() << "'...");
      try {
        // Options:
        std::string path;
        // Formatting options:
        _all_       = false;
        _classify_  = false;
        _longf_     = false;
        _colored_   = false;
        _dirs_only_  = false;
        _dirs_first_ = false;
        _full_path_ = false;
        _prune_empty_dirs_ = false;
        _max_level_ = 0;
        _level_ = 0;
        const basic_shell & shell = _get_target();
        if (_grab_vmap().count("path")) {
          path = _grab_vmap()["path"].as<std::string>();
        }
        if (path.empty()) {
          path = datatools::ui::path::current_interface_path();
        }
        if (_grab_vmap().count("all")) {
          _all_ = true;
        }
        if (_grab_vmap().count("classify")) {
          _classify_ = true;
        }
        if (_grab_vmap().count("long")) {
          _longf_ = true;
        }
        if (_grab_vmap().count("color")) {
          _colored_ = true;
        }
        if (_grab_vmap().count("full-path")) {
          _full_path_ = true;
        }
        if (_grab_vmap().count("dirsonly")) {
          _dirs_only_ = true;
        }
        if (_grab_vmap().count("dirsfirst")) {
          _dirs_first_ = true;
        }
        if (_grab_vmap().count("prune")) {
          _prune_empty_dirs_ = true;
        }
        if (_grab_vmap().count("level")) {
          _max_level_ = _grab_vmap()["level"].as<uint32_t>();
        }
        std::string full_path = shell.canonical_path(path);
        if (! shell.get_ihs().exists(full_path)) {
          DT_COMMAND_RETURNED_ERROR(cri_,
                                    datatools::command::CEC_FAILURE,
                                    get_name() << ": Path '" << path << "' "
                                    << "('" << full_path << "') does not exists!");
        } else {
          // std::cerr << "DEVEL: path      = '" << path << "'" << std::endl;
          // std::cerr << "DEVEL: full_path = '" << full_path << "'" << std::endl;
          // std::string formated_path = _format_full_path(full_path, FM_ORIGINAL);
          std::string formated_path = _format_full_path(path, FM_ORIGINAL);
          std::cout << formated_path << std::endl;
          if (shell.get_ihs().is_interface(full_path)) {
            bool print_children = true;
            if (_prune_empty_dirs_) {
              if (! shell.get_ihs().has_children(full_path)) {
                print_children = false;
              }
            }
            if (print_children) {
              _tree_print_children(std::cout, full_path, "");
            }
          }
        }
      } catch (std::exception & error) {
        DT_COMMAND_RETURNED_ERROR(cri_,
                                  datatools::command::CEC_FAILURE,
                                  get_name() + ": " + error.what());
      }
      DT_LOG_TRACE(get_logging_priority(), "End of command '" << get_name() << "'.");
      return;
    }

    std::string shell_tree_command::_format_full_path(const std::string & full_path_,
                                                      format_mode_type fm_)
    {
      const basic_shell & shell = _get_target();
      std::string orig_path;
      std::string full_path = full_path_;
      if (fm_ == FM_ORIGINAL) {
        orig_path = full_path_;
        if (orig_path.empty()) {
          orig_path = datatools::ui::path::current_interface_path();
        }
        full_path = shell.canonical_path(orig_path);
      }
      std::string parent_path = datatools::ui::path::parent_path(full_path);
      std::string base_path   = datatools::ui::path::basename(full_path);
      bool interface = false;
      bool command   = false;
      bool disabled  = false;
      bool broken    = false;
      bool hidden    = false;
      std::ostringstream out;
      if (shell.get_ihs().is_trait(full_path, traits::hidden_label())) {
        hidden = true;
      }
      if (shell.get_ihs().is_interface(full_path)) {
        interface = true;
      } else {
        command = true;
      }
      if (shell.get_ihs().is_trait(full_path, traits::disabled_label())) {
        disabled = true;
      }
      if (shell.get_ihs().is_trait(full_path, traits::broken_label())) {
        broken = true;
      }
      std::string classification_suffix;
      if (interface) {
        if (_classify_) {
          if (full_path != ::datatools::ui::path::root_path()) {
            classification_suffix = ::datatools::ui::path::sep();
          }
        }
      } else if (command) {
        if (_classify_) {
          classification_suffix = ::datatools::ui::path::exec();
        }
      }
      if (!hidden) {
        if (_colored_) {
          if (interface) {
            if (broken) {
              out << ::datatools::ui::ansi_colors::color_8bits(5,0,0);
            } else if (disabled) {
              out << ::datatools::ui::ansi_colors::color_8bits(datatools::ui::ansi_colors::color_8bits::MODE_GRAYSCALE, 12);
            } else {
              out << ::datatools::ui::ansi_colors::color_8bits(0,0,5);
            }
          } else if (command) {
            if (broken) {
              out << ::datatools::ui::ansi_colors::color_8bits(5,0,0);
            } else if (disabled) {
              out << ::datatools::ui::ansi_colors::color_8bits(datatools::ui::ansi_colors::color_8bits::MODE_GRAYSCALE, 12);
            } else {
              out << ::datatools::ui::ansi_colors::color_8bits(0,5,0);
            }
          }
        }
        if (fm_ == FM_BASENAME) {
          out << base_path;
        } else if (fm_ == FM_FULLPATH) {
          out << full_path;
        } else if (fm_ == FM_ORIGINAL) {
          out << orig_path;
        }
        if (_colored_) {
          out << ::datatools::ui::ansi_colors::reset();
        }
        if (_classify_) {
          out << classification_suffix;
        }
      }
      return out.str();
    }

    void shell_tree_command::_tree_print_children(std::ostream & out_,
                                                  const std::string & path_,
                                                  const std::string & indent_)
    {
      const basic_shell & shell = _get_target();
      std::string full_path = shell.canonical_path(path_);
      std::vector<std::string> children;
      shell.get_ihs().build_children_paths(full_path, children);
      _level_++;
      std::map<std::string, bool> listed_children;
      std::size_t nb_child_interfaces = 0;
      for (int i = 0; i < (int) children.size(); i++) {
        // std::cerr << "DEVEL: full_path = '" << full_path << "' : child = '" << children[i] << "'" << std::endl;
        bool list_it = true;
        std::string child_full_path = shell.canonical_path(children[i]);
        std::string child_path = datatools::ui::path::basename(children[i]);
        bool child_hidden    = false;
        bool child_interface = false;
        bool child_command   = false;
        if (shell.get_ihs().is_trait(child_full_path, traits::hidden_label())) {
          child_hidden = true;
        }
        if (shell.get_ihs().is_interface(child_full_path)) {
          child_interface = true;
        } else {
          child_command = true;
        }
        if (child_hidden && !_all_) {
          list_it = false;
        }
        if (child_command && _dirs_only_) {
          list_it = false;
        }
        if (_max_level_ && _level_ > _max_level_) {
          list_it = false;
        }
        if (list_it) {
          listed_children[child_full_path] = child_interface;
          if (child_interface) nb_child_interfaces++;
        }
      }
      size_t child_counter = 0;
      for (std::map<std::string, bool>::const_iterator ilisted = listed_children.begin();
           ilisted != listed_children.end();
           ilisted++) {
        const std::string & child_full_path = ilisted->first;
        bool interface = ilisted->second;
        child_counter++;
        format_mode_type fm = FM_BASENAME;
        if (_full_path_) {
          fm = FM_FULLPATH;
        }
        std::string child_formated_path = _format_full_path(child_full_path, fm);
        if (!child_formated_path.empty()) {
          std::string indent2;
          if (!_full_path_) {
            out_ << indent_;
            indent2 = indent_;
            if (child_counter == listed_children.size()) {
              indent2 += "    ";
              out_ << "`-- ";
            } else {
              indent2 += "|   ";
              out_ << "|-- ";
            }
          }
          out_ << child_formated_path;
          out_ << std::endl;
          if (interface) {
            _tree_print_children(out_, child_full_path, indent2);
          }
        }
      }
      _level_--;
      return;
    }

  } // namespace ui

} // namespace datatools
