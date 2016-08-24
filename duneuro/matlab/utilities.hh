#ifndef DUNEURO_MATLAB_UTILITIES_HH
#define DUNEURO_MATLAB_UTILITIES_HH

#include <mex.h>

#include <dune/common/parametertree.hh>

#include <duneuro/common/dense_matrix.hh>
#include <duneuro/common/dipole.hh>

namespace duneuro
{
  /** \TODO docme! */
  template <class T>
  mxArray* convert_ptr_to_mat(T* ptr)
  {
    mxArray* out = mxCreateNumericMatrix(1, 1, mxUINT64_CLASS, mxREAL);
    *((uint64_t*) mxGetData(out)) = reinterpret_cast<uint64_t>(ptr);
    return out;
  }

  /** \TODO docme! */
  template <class T>
  T* convert_mat_to_ptr(const mxArray* in)
  {
    if (mxGetNumberOfElements(in) != 1 || mxGetClassID(in) != mxUINT64_CLASS || mxIsComplex(in))
      mexErrMsgTxt("Input must be a real uint64 scalar.");
    return reinterpret_cast<T*>(*((uint64_t*) mxGetData(in)));
  }

  Dune::ParameterTree matlab_struct_to_parametertree(const mxArray* mstr);
  /**
   * \brief extract a single dipole from a matlab array
   *
   * the matlab array is assumed to have 6 double elements, encoding the position and moment x,y and
   * z component (px,py,pz,mx,my,mz)
   */
  Dipole<double, 3> extract_dipole(const mxArray* arr);

  /**
   * \brief extract dipoles from a matlab array
   *
   * the array is assumed to have a size of 6xN where N is the number of dipoles. Each column
   * consist of the position and moment of a single dipole. its format should be (px,py,pz,mx,my,mz)
   */
  std::vector<Dipole<double, 3>> extract_dipoles(const mxArray* arr);

  /** \TODO docme! */
  std::vector<Dune::FieldVector<double, 3>> extract_field_vectors(const mxArray* arr);

  /** \TODO docme! */
  std::vector<std::vector<Dune::FieldVector<double, 3>>> extract_projections(const mxArray* arr);

  /** \TODO docme! */
  std::unique_ptr<const DenseMatrix<double>> extract_dense_matrix(mxArray* arr);

  /** \TODO docme! */
  bool extract_bool(const mxArray* arr);
}

#endif // DUNEURO_MATLAB_UTILITIES_HH
