matlab_add_mex(NAME duneuro_matlab SRC duneuro-matlab.cc
  ${CMAKE_SOURCE_DIR}/duneuro/matlab/utilities.cc
  ${CMAKE_SOURCE_DIR}/duneuro/matlab/command_handler.cc)
dune_symlink_to_source_files(FILES duneuro_meeg.m)
dune_symlink_to_source_files(FILES duneuro_function.m)
