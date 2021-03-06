// -*- mode: c++ ; -*-
// test_data_io.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <boost/filesystem.hpp>

#include <brio/writer.h>
#include <brio/reader.h>
#include <brio_test_data.cc>

// Serialization code :
#include <brio_test_data.ipp>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for serialization of the 'brio::test::data_t' sample class !" << endl;

      bool delete_file = false;
      size_t data_count = 10;
      int iarg = 1;
      while (iarg < argc_)
        {
          string token = argv_[iarg];

          if (token[0] == '-')
            {
               string option = token;
               if ((option == "-m") || (option == "--many"))
                 {
                   data_count = 1000;
                 }
               else if ((option == "-M") || (option == "--manymany"))
                 {
                   data_count = 100000;
                 }
               else if ((option == "-F") || (option == "--delete-file"))
                 {
                   delete_file = true;
                 }
               else
                 {
                    clog << "warning: ignoring option '" << option << "'!" << endl;
                 }
            }
          else
            {
              string argument = token;
              {
                clog << "warning: ignoring argument '" << argument << "'!" << endl;
              }
            }
          iarg++;
      }

      {
        clog << "notice: Store data..." << endl;
        // Declare a brio writer:
        brio::writer my_writer ("test_data_io.brio");
        // Store `data' objects within the *automatic* store;
        for (int i = 0; i < (int) data_count; i++)
          {
            brio::test::data_t a_data;
            my_writer.store (a_data);
          }
        my_writer.close ();
        clog << "notice: Done." << endl;
      }

      {
        clog << "notice: Load data..." << endl;
        // Declare a brio reader:
        brio::reader my_reader ("test_data_io.brio");

        // Load `data' objects from the *automatic* store;
        while (my_reader.has_next ())
          {
            brio::test::data_t a_data;
            my_reader.load_next (a_data);
          }
        my_reader.close ();
        clog << "notice: Done." << endl;
      }

      if (delete_file)
        {
          if (boost::filesystem::exists ("test_data_io.brio"))
            {
              unlink ("test_data_io.brio");
            }
        }

    }
  catch (exception & x)
    {
      cerr << "error: " << x.what () << endl;
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      cerr << "error: " << "unexpected error!" << endl;
      error_code = EXIT_FAILURE;
    }
  return (error_code);
}
