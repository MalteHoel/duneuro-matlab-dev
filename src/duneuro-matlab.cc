// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstdint>
#include <iostream>

#include <mex.h>

#include <dune/common/exceptions.hh>

#include <duneuro/matlab/command_handler.hh>

void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
  try {
    if (nrhs == 0) {
      mexErrMsgTxt("please provide a command argument");
      return;
    }
    duneuro::CommandHandler::run_command(nlhs, plhs, nrhs, prhs);
  } catch (Dune::Exception& ex) {
    mexErrMsgTxt(ex.what());
    return;
  }
  return;
}
