duneuro-matlab
=============

this module provides matlab bindings for the duneuro module.

As a prerequisite, you need to have matlab installed (in `<your-matlab-dir>` ,
e.g. `/usr/local/matlab/R2016a`). Then you need to point cmake to this directory
by setting the cmake variable `MATLAB_ROOT=<your-matlab-dir>`. This can for
example be done in the options file that is passed to `dunecontrol` by adding
`-DMatlab_ROOT_DIR=<your-matlab-dir>` to the `CMAKE_FLAGS` variable.