# - Module definition for materials submodule of Bayeux
#
# To configure the module headers, the variable
# MODULE_HEADER_ROOT must be set before including this file

foreach(_modulevar MODULE_HEADER_ROOT MODULE_RESOURCE_ROOT)
  if(NOT ${_modulevar})
    message(FATAL_ERROR "${_modulevar} not specified")
  endif()
endforeach()

# - Module
set(module_name materials)
set(module_root_dir "${CMAKE_CURRENT_SOURCE_DIR}/bx${module_name}")
set(module_include_dir "${module_root_dir}/include")
set(module_source_dir  "${module_root_dir}/src")
set(module_test_dir    "${module_root_dir}/testing")
set(module_resource_dir "${module_root_dir}/resources")

foreach(dir root_dir include_dir source_dir test_dir resource_dir)
  set(${module_name}_${dir} ${module_${dir}})
endforeach()

# - In place defs for module CMake variables...
# - Versioning
set(materials_VERSION_MAJOR 4)
set(materials_VERSION_MINOR 0)
set(materials_PATCH_VERSION 0)
set(materials_VERSION "${materials_VERSION_MAJOR}.${materials_VERSION_MINOR}.${materials_PATCH_VERSION}")


# - Raw Headers and Sources
set(${module_name}_MODULE_HEADERS
  ${module_include_dir}/${module_name}/chemical_symbol.h
  ${module_include_dir}/${module_name}/detail/tools.h
  ${module_include_dir}/${module_name}/element.h
  ${module_include_dir}/${module_name}/factory.h
  ${module_include_dir}/${module_name}/isotope.h
  ${module_include_dir}/${module_name}/manager.h
  ${module_include_dir}/${module_name}/material.h
  ${module_include_dir}/${module_name}/materials_driver.h
  ${module_include_dir}/${module_name}/materials_config.h.in
  ${module_include_dir}/${module_name}/resource.h
  )

# - configure resources
configure_file(${module_source_dir}/resource.cc.in resource.cc)

set(${module_name}_MODULE_SOURCES
  ${module_source_dir}/chemical_symbol.cc
  ${module_source_dir}/element.cc
  ${module_source_dir}/factory.cc
  ${module_source_dir}/isotope.cc
  ${module_source_dir}/material.cc
  ${module_source_dir}/manager.cc
  ${module_source_dir}/materials_driver.cc
  resource.cc
  )

# - Published headers
foreach(_hdrin ${${module_name}_MODULE_HEADERS})
  string(REGEX REPLACE "\\.in$" "" _hdrout "${_hdrin}")
  string(REGEX REPLACE "^${module_include_dir}" "${MODULE_HEADER_ROOT}" _hdrout "${_hdrout}")
  configure_file(${_hdrin} ${_hdrout} @ONLY)
endforeach()

# - Unit tests
set(${module_name}_TEST_ENVIRONMENT "MATERIALS_DATA_DIR=${module_root_dir}")

set(${module_name}_MODULE_TESTS
  ${module_test_dir}/test_chemical_symbol.cxx
  ${module_test_dir}/test_element.cxx
  ${module_test_dir}/test_factory.cxx
  ${module_test_dir}/test_isotope.cxx
  ${module_test_dir}/test_manager_2.cxx
  ${module_test_dir}/test_manager.cxx
  ${module_test_dir}/test_material.cxx
  ${module_test_dir}/test_materials.cxx
  ${module_test_dir}/test_resource.cxx
  )

# - Resource files
set(${module_name}_MODULE_RESOURCES
  ${module_resource_dir}/data/isotopic_compositions_nist.dat
  ${module_resource_dir}/data/JEFF311RDD_ALL.OUT
  ${module_resource_dir}/data/mass.mas03
  ${module_resource_dir}/data/README.txt
  ${module_resource_dir}/data/simple_elements.def
  ${module_resource_dir}/data/std_elements.def
  ${module_resource_dir}/data/std_isotopes.def
  ${module_resource_dir}/data/std_materials.def
  )

# - Publish resource files
foreach(_rfin ${${module_name}_MODULE_RESOURCES})
  string(REGEX REPLACE "\\.in$" "" _rfout "${_rfin}")
  string(REGEX REPLACE "^${module_resource_dir}" "${MODULE_RESOURCE_ROOT}" _rfout "${_rfout}")
  configure_file(${_rfin} ${_rfout} @ONLY)
endforeach()
