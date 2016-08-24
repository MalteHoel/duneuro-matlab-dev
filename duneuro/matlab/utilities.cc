#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <duneuro/matlab/utilities.hh>

#include <dune/common/std/memory.hh>

namespace duneuro
{
  std::map<std::string, std::string> matlab_struct_to_map(const mxArray* mstr)
  {
    std::map<std::string, std::string> out;

    auto nfields = mxGetNumberOfFields(mstr);
    for (int i = 0; i < nfields; ++i) {
      std::string name(mxGetFieldNameByNumber(mstr, i));
      auto field = mxGetFieldByNumber(mstr, 0, i);
      if (mxIsStruct(field)) {
        auto sub = matlab_struct_to_map(field);
        for (const auto& k : sub) {
          out[name + '.' + k.first] = k.second;
        }
      } else if (mxIsChar(field)) {
        out[name] = std::string(mxArrayToString(field));
      } else {
        std::stringstream sstr;
        sstr << "data type for \"" << name << "\" not supported";
        auto str = sstr.str();
        mexErrMsgTxt(str.c_str());
      }
    }

    return out;
  }

  Dune::ParameterTree matlab_struct_to_parametertree(const mxArray* mstr)
  {
    Dune::ParameterTree config;
    auto map = matlab_struct_to_map(mstr);
    for (const auto& k : map) {
      config[k.first] = k.second;
    }
    return config;
  }

  Dipole<double, 3> extract_dipole(const mxArray* arr)
  {
    if (!mxIsDouble(arr)) {
      mexErrMsgTxt("expected double matrix for dipole");
    }
    int N = mxGetNumberOfElements(arr);
    if (N != 6) {
      mexErrMsgTxt("number of elements has to be two times the number of dims, i.e. 6");
    }
    const double* ptr = mxGetPr(arr);
    Dune::FieldVector<double, 3> pos, mom;
    std::copy(ptr, ptr + 3, pos.begin());
    std::copy(ptr + 3, ptr + 6, mom.begin());
    return Dipole<double, 3>(pos, mom);
  }

  std::vector<Dipole<double, 3>> extract_dipoles(const mxArray* arr)
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
    std::vector<Dipole<double, 3>> output;
    for (int i = 0; i < cols; ++i, ptr += rows) {
      Dune::FieldVector<double, 3> pos, mom;
      std::copy(ptr, ptr + 3, pos.begin());
      std::copy(ptr + 3, ptr + 6, mom.begin());
      std::cout << "pos: " << pos << " mom: " << mom << std::endl;
      output.push_back(Dipole<double, 3>(pos, mom));
    }
    return output;
  }

  std::vector<Dune::FieldVector<double, 3>> extract_field_vectors(const mxArray* arr)
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

  std::vector<std::vector<Dune::FieldVector<double, 3>>> extract_projections(const mxArray* arr)
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

  std::unique_ptr<const DenseMatrix<double>> extract_dense_matrix(mxArray* arr)
  {
    if (!mxIsDouble(arr)) {
      mexErrMsgTxt("expected double matrix for electrodes");
    }
    int rows = mxGetM(arr);
    int cols = mxGetN(arr);
    double* ptr = mxGetPr(arr);
    return Dune::Std::make_unique<DenseMatrix<double>>(cols, rows, ptr);
  }

  bool extract_bool(const mxArray* arr)
  {
    if (!mxIsLogicalScalar(arr)) {
      mexErrMsgTxt("expected logical scalar");
    }
    return mxIsLogicalScalarTrue(arr);
  }
}
