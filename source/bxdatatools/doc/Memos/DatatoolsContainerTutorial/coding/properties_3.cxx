#include <iostream>

#include <datatools/utils/properties.h>

// A predicate on 'string' that searches for a given substring :
class contains_string_predicate : public std::unary_function<const string &, bool>
{
  string _token_; // the substring to be found in the string argument
public:
  contains_string_predicate (const string & token_) : _token_ (token_)
  {
  }
  bool operator () (const string & key_)const
  {
    return key_.find (_token_) != key_.npos;
  }
};

int main (void)
{
  datatools::utils::properties config ("A test properties container");
  config.store_flag ("test");
  config.store_flag ("program_name", "prog");
  config.store ("debug.level", 1);
  config.store ("debug.filename", "debug.log");
  config.tree_dump (std::cout, "Original container :");
  std::cout << endl;

  datatools::utils::properties debug_config;
  config.export_starting_with (debug_config, "debug.");
  debug_config.tree_dump (std::cout, 
    "Exported properties container with the 'debug.' key prefix :");
  std::cout << endl;

  datatools::utils::properties non_debug_config;
  config.export_not_starting_with (non_debug_config, "debug.");
  non_debug_config.tree_dump (std::cout, 
    "Exported properties container without the 'debug.' key prefix :");
  std::cout << endl;

  datatools::utils::properties with_name_config;
  config.export_if (with_name_config, contains_string_predicate ("name"));
  with_name_config.tree_dump (std::cout, 
    "Exported properties container with the 'name' string in key :");
  std::cout << endl;
  
  datatools::utils::properties without_name_config;
  config.export_not_if (without_name_config, contains_string_predicate ("name"));
  without_name_config.tree_dump (std::cout, 
    "Exported properties container without the 'name' string in key :");
  std::cout << endl;

  return 0;
}
