matlab_add_mex(NAME duneuro_matlab SRC duneuro-matlab.cc
  ${CMAKE_SOURCE_DIR}/duneuro/matlab/utilities.cc
  ${CMAKE_SOURCE_DIR}/duneuro/matlab/command_handler.cc)
set_target_properties(duneuro_matlab PROPERTIES COMPILE_FLAGS "-fvisibility=default")
dune_symlink_to_source_files(FILES duneuro_meeg.m)
dune_symlink_to_source_files(FILES duneuro_function.m)
dune_symlink_to_source_files(FILES duneuro_volume_vtk_writer.m)
dune_symlink_to_source_files(FILES duneuro_point_vtk_writer.m)
