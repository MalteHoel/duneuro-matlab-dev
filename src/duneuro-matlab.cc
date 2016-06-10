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

#include <duneuro/meeg/meeg_driver_factory.hh>

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

std::vector<Dune::FieldVector<double, 3>> extractFieldVectors(const mxArray* arr)
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

std::vector<std::vector<Dune::FieldVector<double, 3>>> extractProjections(const mxArray* arr)
{
  if (!mxIsDouble(arr)) {
    mexErrMsgTxt("expected double matrix for electrodes");
  }
  int rows = mxGetM(arr);
  int cols = mxGetN(arr);
  if (rows % 3 != 0) {
    mexErrMsgTxt("number of rows has to be the a multiple of the number of dims, i.e. 3");
  }
  const double* ptr = mxGetPr(arr);
  std::vector<std::vector<Dune::FieldVector<double, 3>>> output;
  for (int i = 0; i < cols; ++i, ptr += rows) {
    std::vector<Dune::FieldVector<double, 3>> current;
    for (int j = 0; j < rows; j += 3) {
      Dune::FieldVector<double, 3> v;
      std::copy(ptr + j, ptr + j + 3, v.begin());
      std::cout << "pos: " << v << std::endl;
      current.push_back(v);
    }
    output.push_back(current);
  }
  return output;
}

std::unique_ptr<const duneuro::DenseMatrix<double>> extractDenseMatrix(mxArray* arr)
{
  if (!mxIsDouble(arr)) {
    mexErrMsgTxt("expected double matrix for electrodes");
  }
  int rows = mxGetM(arr);
  int cols = mxGetN(arr);
  double* ptr = mxGetPr(arr);
  return Dune::Std::make_unique<duneuro::DenseMatrix<double>>(cols, rows, ptr);
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
      plhs[0] = convertPtr2Mat(duneuro::MEEGDriverFactory::make_meeg_driver(config).release());
      std::cout << "created and lock called\n";
    } else if (command == "solve_eeg_forward") {
      if (nrhs < 4) {
        mexErrMsgTxt("please provide a handle to the object, the electrodes and the dipoles");
        return;
      }
      if (nlhs != 1) {
        mexErrMsgTxt("the method returns a matrix");
        return;
      }
      auto* foo = convertMat2Ptr<duneuro::MEEGDriverInterface>(prhs[1]);
      auto electrodes = extractFieldVectors(prhs[2]);
      auto dipoles = extractDipoles(prhs[3]);
      foo->setElectrodes(electrodes);
      std::cout << "creating matrix with " << electrodes.size() << " rows and " << dipoles.size()
                << " cols\n";
      plhs[0] = mxCreateDoubleMatrix(electrodes.size(), dipoles.size(), mxREAL);
      auto ptr = mxGetPr(plhs[0]);
      auto solution = foo->makeDomainFunction();
      for (unsigned int i = 0; i != dipoles.size(); ++i, ptr += electrodes.size()) {
        foo->solveEEGForward(dipoles[i], solution);
        auto ae = foo->evaluateAtElectrodes(solution);
        std::copy(ae.begin(), ae.end(), ptr);
      }
    } else if (command == "solve_meg_forward") {
      if (nrhs < 5) {
        mexErrMsgTxt(
            "please provide a handle to the object, the coils, the projections and the dipoles");
        return;
      }
      if (nlhs != 1) {
        mexErrMsgTxt("the method returns a matrix");
        return;
      }
      auto* foo = convertMat2Ptr<duneuro::MEEGDriverInterface>(prhs[1]);
      auto coils = extractFieldVectors(prhs[2]);
      auto projections = extractProjections(prhs[3]);
      std::size_t numberOfProjections = 0;
      for (const auto& p : projections)
        numberOfProjections += p.size();
      auto dipoles = extractDipoles(prhs[4]);
      foo->setCoilsAndProjections(coils, projections);
      std::cout << "creating matrix with " << numberOfProjections << " rows and " << dipoles.size()
                << " cols\n";
      plhs[0] = mxCreateDoubleMatrix(numberOfProjections, dipoles.size(), mxREAL);
      auto ptr = mxGetPr(plhs[0]);
      auto solution = foo->makeDomainFunction();
      for (unsigned int i = 0; i != dipoles.size(); ++i, ptr += numberOfProjections) {
        foo->solveEEGForward(dipoles[i], solution);
        auto ae = foo->solveMEGForward(solution);
        std::copy(ae.begin(), ae.end(), ptr);
      }
    } else if (command == "compute_eeg_transfer_matrix") {
      if (nrhs < 3) {
        mexErrMsgTxt("please provide a handle to the object and the electrodes");
        return;
      }
      if (nlhs != 1) {
        mexErrMsgTxt("the method returns a matrix");
        return;
      }
      auto* foo = convertMat2Ptr<duneuro::MEEGDriverInterface>(prhs[1]);
      auto electrodes = extractFieldVectors(prhs[2]);
      foo->setElectrodes(electrodes);
      auto tm = foo->computeEEGTransferMatrix();
      plhs[0] = mxCreateDoubleMatrix(tm->cols(), tm->rows(), mxREAL);
      auto ptr = mxGetPr(plhs[0]);
      std::copy(tm->data(), tm->data() + tm->rows() * tm->cols(), ptr);
    } else if (command == "compute_meg_transfer_matrix") {
      if (nrhs < 4) {
        mexErrMsgTxt("please provide a handle to the object, the coils and the projections");
        return;
      }
      if (nlhs != 1) {
        mexErrMsgTxt("the method returns a matrix");
        return;
      }
      auto* foo = convertMat2Ptr<duneuro::MEEGDriverInterface>(prhs[1]);
      auto coils = extractFieldVectors(prhs[2]);
      auto projections = extractProjections(prhs[3]);
      foo->setCoilsAndProjections(coils, projections);
      auto tm = foo->computeMEGTransferMatrix();
      plhs[0] = mxCreateDoubleMatrix(tm->cols(), tm->rows(), mxREAL);
      auto ptr = mxGetPr(plhs[0]);
      std::copy(tm->data(), tm->data() + tm->rows() * tm->cols(), ptr);
    } else if (command == "apply_transfer") {
      if (nrhs < 4) {
        mexErrMsgTxt("please provide a handle to the object, the transfer matrix and the dipoles");
        return;
      }
      if (nlhs != 1) {
        mexErrMsgTxt("the method returns a matrix");
        return;
      }
      auto* foo = convertMat2Ptr<duneuro::MEEGDriverInterface>(prhs[1]);
      // the const cast below is a work around to fulfill the dense matrix interface.
      auto tm = extractDenseMatrix(const_cast<mxArray*>(prhs[2]));
      auto dipoles = extractDipoles(prhs[3]);
      plhs[0] = mxCreateDoubleMatrix(tm->rows(), dipoles.size(), mxREAL);
      auto ptr = mxGetPr(plhs[0]);
      for (unsigned int i = 0; i != dipoles.size(); ++i, ptr += tm->rows()) {
        auto ae = foo->applyTransfer(*tm, dipoles[i]);
        std::copy(ae.begin(), ae.end(), ptr);
      }
    } else if (command == "delete") {
      if (nrhs < 2) {
        mexErrMsgTxt("please provide a handle to the object");
        return;
      }
      auto* foo = convertMat2Ptr<duneuro::MEEGDriverInterface>(prhs[1]);
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
