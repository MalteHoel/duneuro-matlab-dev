// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cstdint>
#include <iostream>

#include <mex.h>

#include <dune/common/exceptions.hh>
#include <dune/common/parametertree.hh>

#include <duneuro/eeg/eeg_driver_factory.hh>

template <class T>
mxArray* convertPtr2Mat(T* ptr)
{
  mxArray* out = mxCreateNumericMatrix(1, 1, mxUINT64_CLASS, mxREAL);
  *((uint64_t*) mxGetData(out)) = reinterpret_cast<uint64_t>(ptr);
  return out;
}

template <class T>
T* convertMat2Ptr(const mxArray* in)
{
  if (mxGetNumberOfElements(in) != 1 || mxGetClassID(in) != mxUINT64_CLASS || mxIsComplex(in))
    mexErrMsgTxt("Input must be a real uint64 scalar.");
  return reinterpret_cast<T*>(*((uint64_t*) mxGetData(in)));
}

Dune::ParameterTree matlabStructToParameterTree(const mxArray* mstr)
{
  Dune::ParameterTree config;
  auto nfields = mxGetNumberOfFields(mstr);
  for (int i = 0; i < nfields; ++i) {
    std::string name(mxGetFieldNameByNumber(mstr, i));
    auto field = mxGetFieldByNumber(mstr, 0, i);
    if (mxIsStruct(field)) {
      auto& sub = config.sub(name);
      sub = matlabStructToParameterTree(field);
    } else if (mxIsChar(field)) {
      config[name] = std::string(mxArrayToString(field));
    } else {
      std::stringstream sstr;
      sstr << "data type for \"" << name << "\" not supported";
      auto str = sstr.str();
      mexErrMsgTxt(str.c_str());
    }
  }
  return config;
}

std::vector<duneuro::Dipole<double, 3>> extractDipoles(const mxArray* arr)
{
  if (!mxIsDouble(arr)) {
    mexErrMsgTxt("expected double matrix for dipoles");
  }
  int rows = mxGetM(arr);
  int cols = mxGetN(arr);
  if (rows != 6) {
    mexErrMsgTxt("number of rows has to be two times the number of dims, i.e. 6");
  }
  const double* ptr = mxGetPr(arr);
  std::vector<duneuro::Dipole<double, 3>> output;
  for (int i = 0; i < cols; ++i, ptr += rows) {
    Dune::FieldVector<double, 3> pos, mom;
    std::copy(ptr, ptr + 3, pos.begin());
    std::copy(ptr + 3, ptr + 6, mom.begin());
    std::cout << "pos: " << pos << " mom: " << mom << std::endl;
    output.push_back(duneuro::Dipole<double, 3>(pos, mom));
  }
  return output;
}

std::vector<Dune::FieldVector<double, 3>> extractElectrodes(const mxArray* arr)
{
  if (!mxIsDouble(arr)) {
    mexErrMsgTxt("expected double matrix for electrodes");
  }
  int rows = mxGetM(arr);
  int cols = mxGetN(arr);
  if (rows != 3) {
    mexErrMsgTxt("number of rows has to be the number of dims, i.e. 3");
  }
  const double* ptr = mxGetPr(arr);
  std::vector<Dune::FieldVector<double, 3>> output;
  for (int i = 0; i < cols; ++i, ptr += rows) {
    Dune::FieldVector<double, 3> v;
    std::copy(ptr, ptr + rows, v.begin());
    std::cout << "pos: " << v << std::endl;
    output.push_back(v);
  }
  return output;
}

void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
  try {
    if (nrhs == 0) {
      mexErrMsgTxt("please provide a command argument");
      return;
    }
    std::string command(mxArrayToString(prhs[0]));
    if (command == "create") {
      if (nlhs != 1) {
        mexErrMsgTxt("the method returns a handle");
        return;
      }
      if (nrhs != 2) {
        mexErrMsgTxt("one input required");
      }
      mexLock();
      auto config = matlabStructToParameterTree(prhs[1]);
      plhs[0] = convertPtr2Mat(duneuro::EEGDriverFactory::make_eeg_driver(config).release());
      std::cout << "created and lock called\n";
    } else if (command == "solve_direct") {
      if (nrhs < 3) {
        mexErrMsgTxt("please provide a handle to the object and the dipoles");
        return;
      }
      if (nlhs != 1) {
        mexErrMsgTxt("the method returns a matrix");
        return;
      }
      auto* foo = convertMat2Ptr<duneuro::EEGDriverInterface>(prhs[1]);
      auto electrodes = extractElectrodes(prhs[2]);
      auto dipoles = extractDipoles(prhs[3]);
      foo->setElectrodes(electrodes);
      std::cout << "creating matrix with " << electrodes.size() << " rows and " << dipoles.size() << " cols\n";
      plhs[0] = mxCreateDoubleMatrix(electrodes.size(), dipoles.size(), mxREAL);
      auto ptr = mxGetPr(plhs[0]);
      auto solution = foo->makeDomainFunction();
      for (unsigned int i = 0; i != dipoles.size(); ++i, ptr += electrodes.size()) {
        foo->solve(dipoles[i], solution);
        auto ae = foo->evaluateAtElectrodes(solution);
        std::copy(ae.begin(), ae.end(), ptr);
      }
    } else if (command == "delete") {
      if (nrhs < 2) {
        mexErrMsgTxt("please provide a handle to the object");
        return;
      }
      auto* foo = convertMat2Ptr<duneuro::EEGDriverInterface>(prhs[1]);
      delete foo;
      mexUnlock();
      std::cout << "deleted and unlock called\n";
    }
  } catch (Dune::Exception& ex) {
    mexErrMsgTxt(ex.what().c_str());
    return;
  }
  return;
}
