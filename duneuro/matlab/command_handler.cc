#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <duneuro/matlab/command_handler.hh>

#include <duneuro/eeg/eeg_analytic_solution.hh>
#include <duneuro/meeg/meeg_driver_factory.hh>

#include <duneuro/matlab/utilities.hh>

namespace duneuro
{
  void CommandHandler::create_driver(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
  {
    if (nlhs != 1) {
      mexErrMsgTxt("the method returns a handle");
      return;
    }
    if (nrhs != 1) {
      mexErrMsgTxt("one input required");
    }
    auto config = matlab_struct_to_parametertree(prhs[0]);
    config.report(std::cout);
    plhs[0] = convert_ptr_to_mat(MEEGDriverFactory<3>::make_meeg_driver(config).release());
    // note: mexLock has a lock count, call mexUnlock each time a driver is destroyed
    mexLock();
  }

  void CommandHandler::make_domain_function(int nlhs, mxArray* plhs[], int nrhs,
                                            const mxArray* prhs[])
  {
    if (nrhs != 1) {
      mexErrMsgTxt("one input required");
    }
    auto* foo = convert_mat_to_ptr<MEEGDriverInterface<3>>(prhs[0]);
    plhs[0] = convert_ptr_to_mat(foo->makeDomainFunction().release());
    // note: mexLock has a lock count, call mexUnlock each time a function is destroyed
    mexLock();
  }

  void CommandHandler::delete_function(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
  {
    if (nrhs != 1) {
      mexErrMsgTxt("please provide a function handle");
      return;
    }
    auto* foo = convert_mat_to_ptr<Function>(prhs[0]);
    delete foo;
    mexUnlock();
  }

  void CommandHandler::set_source_model(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
  {
    if (nrhs < 2) {
      mexErrMsgTxt(
          "please provide a handle to the object and a configuration struct");
      return;
    }
    if (nlhs != 0) {
      mexErrMsgTxt("the method does not return variables");
      return;
    }
    auto* foo = convert_mat_to_ptr<MEEGDriverInterface<3>>(prhs[0]);
    foo->setSourceModel(matlab_struct_to_parametertree(prhs[1]));
  }

  void CommandHandler::solve_eeg_forward(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
  {
    if (nrhs < 4) {
      mexErrMsgTxt(
          "please provide a handle to the object, the dipole, the solution function and a "
          "configuration struct");
      return;
    }
    if (nlhs != 0) {
      mexErrMsgTxt("the method does not return variables");
      return;
    }
    auto* foo = convert_mat_to_ptr<MEEGDriverInterface<3>>(prhs[0]);
    auto* solution = convert_mat_to_ptr<Function>(prhs[2]);
    foo->solveEEGForward(extract_dipole(prhs[1]), *solution,
                         matlab_struct_to_parametertree(prhs[3]));
  }

  void CommandHandler::solve_meg_forward(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
  {
    if (nrhs < 3) {
      mexErrMsgTxt(
          "please provide a handle to the object, a handle to the eeg solution and a configuration "
          "struct");
      return;
    }
    if (nlhs != 1) {
      mexErrMsgTxt("the method returns a matrix");
      return;
    }
    auto* foo = convert_mat_to_ptr<MEEGDriverInterface<3>>(prhs[0]);
    auto* sol = convert_mat_to_ptr<Function>(prhs[1]);
    auto ae = foo->solveMEGForward(*sol, matlab_struct_to_parametertree(prhs[2]));
    plhs[0] = mxCreateDoubleMatrix(ae.size(), 1, mxREAL);
    std::copy(ae.begin(), ae.end(), mxGetPr(plhs[0]));
  }

  void CommandHandler::compute_eeg_transfer_matrix(int nlhs, mxArray* plhs[], int nrhs,
                                                   const mxArray* prhs[])
  {
    if (nrhs < 2) {
      mexErrMsgTxt("please provide a handle to the object and a configuration struct");
      return;
    }
    if (nlhs != 1) {
      mexErrMsgTxt("the method returns a matrix");
      return;
    }
    auto* foo = convert_mat_to_ptr<MEEGDriverInterface<3>>(prhs[0]);
    auto tm = foo->computeEEGTransferMatrix(matlab_struct_to_parametertree(prhs[1]));
    plhs[0] = mxCreateDoubleMatrix(tm->cols(), tm->rows(), mxREAL);
    std::copy(tm->data(), tm->data() + tm->rows() * tm->cols(), mxGetPr(plhs[0]));
  }

  void CommandHandler::compute_meg_transfer_matrix(int nlhs, mxArray* plhs[], int nrhs,
                                                   const mxArray* prhs[])
  {
    if (nrhs < 2) {
      mexErrMsgTxt("please provide a handle to the object and a configuration struct");
      return;
    }
    if (nlhs != 1) {
      mexErrMsgTxt("the method returns a matrix");
      return;
    }
    auto* foo = convert_mat_to_ptr<MEEGDriverInterface<3>>(prhs[0]);
    auto tm = foo->computeMEGTransferMatrix(matlab_struct_to_parametertree(prhs[1]));
    plhs[0] = mxCreateDoubleMatrix(tm->cols(), tm->rows(), mxREAL);
    std::copy(tm->data(), tm->data() + tm->rows() * tm->cols(), mxGetPr(plhs[0]));
  }

  void CommandHandler::apply_eeg_transfer(int nlhs, mxArray* plhs[], int nrhs,
                                          const mxArray* prhs[])
  {
    if (nrhs < 4) {
      mexErrMsgTxt(
          "please provide a handle to the object, the transfer matrix, the dipole and a "
          "configuration struct");
      return;
    }
    if (nlhs != 1) {
      mexErrMsgTxt("the method returns a matrix");
      return;
    }
    auto* foo = convert_mat_to_ptr<MEEGDriverInterface<3>>(prhs[0]);
    // the const cast below is a work around to fulfill the dense matrix interface.
    auto tm = extract_dense_matrix(const_cast<mxArray*>(prhs[1]));
    auto ae = foo->applyEEGTransfer(*tm, extract_dipole(prhs[2]),
                                    matlab_struct_to_parametertree(prhs[3]));
    plhs[0] = mxCreateDoubleMatrix(tm->rows(), 1, mxREAL);
    std::copy(ae.begin(), ae.end(), mxGetPr(plhs[0]));
  }

  void CommandHandler::apply_meg_transfer(int nlhs, mxArray* plhs[], int nrhs,
                                          const mxArray* prhs[])
  {
    if (nrhs < 4) {
      mexErrMsgTxt(
          "please provide a handle to the object, the transfer matrix, the dipole and a "
          "configuration struct");
      return;
    }
    if (nlhs != 1) {
      mexErrMsgTxt("the method returns a matrix");
      return;
    }
    auto* foo = convert_mat_to_ptr<MEEGDriverInterface<3>>(prhs[0]);
    // the const cast below is a work around to fulfill the dense matrix interface.
    auto tm = extract_dense_matrix(const_cast<mxArray*>(prhs[1]));
    auto ae = foo->applyMEGTransfer(*tm, extract_dipole(prhs[2]),
                                    matlab_struct_to_parametertree(prhs[3]));
    plhs[0] = mxCreateDoubleMatrix(tm->rows(), 1, mxREAL);
    std::copy(ae.begin(), ae.end(), mxGetPr(plhs[0]));
  }

  void CommandHandler::set_electrodes(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
  {
    if (nrhs < 3) {
      mexErrMsgTxt(
          "please provide a handle to the object, the electrodes and a configuration struct");
      return;
    }
    if (nlhs != 0) {
      mexErrMsgTxt("the method does not return variables");
      return;
    }
    auto* foo = convert_mat_to_ptr<MEEGDriverInterface<3>>(prhs[0]);
    foo->setElectrodes(extract_field_vectors(prhs[1]), matlab_struct_to_parametertree(prhs[2]));
  }

  void CommandHandler::set_coils_and_projections(int nlhs, mxArray* plhs[], int nrhs,
                                                 const mxArray* prhs[])
  {
    if (nrhs < 3) {
      mexErrMsgTxt("please provide a handle to the object, the coils and the projections");
      return;
    }
    if (nlhs != 0) {
      mexErrMsgTxt("the method does not return variables");
      return;
    }
    auto* foo = convert_mat_to_ptr<MEEGDriverInterface<3>>(prhs[0]);
    foo->setCoilsAndProjections(extract_field_vectors(prhs[1]), extract_projections(prhs[2]));
  }

  void CommandHandler::evaluate_at_electrodes(int nlhs, mxArray* plhs[], int nrhs,
                                              const mxArray* prhs[])
  {
    if (nrhs < 2) {
      mexErrMsgTxt("please provide a handle to the object and a handle to the function");
      return;
    }
    if (nlhs != 1) {
      mexErrMsgTxt("the method returns a matrix");
      return;
    }
    auto* foo = convert_mat_to_ptr<MEEGDriverInterface<3>>(prhs[0]);
    auto* sol = convert_mat_to_ptr<Function>(prhs[1]);
    auto ae = foo->evaluateAtElectrodes(*sol);
    plhs[0] = mxCreateDoubleMatrix(ae.size(), 1, mxREAL);
    std::copy(ae.begin(), ae.end(), mxGetPr(plhs[0]));
  }

  void CommandHandler::write(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
  {
    if (nrhs < 3) {
      mexErrMsgTxt(
          "please provide a handle to the object, a handle to the function and a configuration "
          "struct");
      return;
    }
    if (nlhs != 0) {
      mexErrMsgTxt("the method does not return variables");
      return;
    }
    auto* foo = convert_mat_to_ptr<MEEGDriverInterface<3>>(prhs[0]);
    auto* sol = convert_mat_to_ptr<Function>(prhs[1]);
    foo->write(*sol, matlab_struct_to_parametertree(prhs[2]));
  }

  void CommandHandler::delete_driver(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
  {
    if (nrhs == 0) {
      mexErrMsgTxt("please provide a handle to the object");
      return;
    }
    auto* foo = convert_mat_to_ptr<MEEGDriverInterface<3>>(prhs[0]);
    delete foo;
    mexUnlock();
  }

  void CommandHandler::eeg_analytical_solution(int nlhs, mxArray* plhs[], int nrhs,
                                               const mxArray* prhs[])
  {
    if (nrhs < 3) {
      mexErrMsgTxt("please provide the configuration, the electrodes and the dipole");
      return;
    }
    if (nlhs != 1) {
      mexErrMsgTxt("the method returns a matrix");
      return;
    }
    auto ptree = matlab_struct_to_parametertree(prhs[0]);
    auto electrodes = extract_field_vectors(prhs[1]);
    auto dipoles = extract_dipoles(prhs[2]);
    auto sol = duneuro::compute_analytic_solution(electrodes, dipoles, ptree);
    plhs[0] = mxCreateDoubleMatrix(sol.rows(), sol.cols(), mxREAL);
    double* ptr = mxGetPr(plhs[0]);
    for (unsigned int j = 0; j < sol.cols(); ++j) {
      for (unsigned int i = 0; i < sol.rows(); ++i) {
        *ptr++ = sol[i][j];
      }
    }
  }

  void CommandHandler::run_command(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
  {
    static std::map<std::string, std::function<void(int, mxArray* [], int, const mxArray* [])>>
        commands = {{"create", create_driver},
                    {"make_domain_function", make_domain_function},
                    {"delete_function", delete_function},
                    {"solve_eeg_forward", solve_eeg_forward},
                    {"solve_meg_forward", solve_meg_forward},
                    {"compute_eeg_transfer_matrix", compute_eeg_transfer_matrix},
                    {"compute_meg_transfer_matrix", compute_meg_transfer_matrix},
                    {"apply_eeg_transfer", apply_eeg_transfer},
                    {"apply_meg_transfer", apply_meg_transfer},
                    {"set_electrodes", set_electrodes},
                    {"set_coils_and_projections", set_coils_and_projections},
                    {"set_source_model", set_source_model},
                    {"evaluate_at_electrodes", evaluate_at_electrodes},
                    {"write", write},
                    {"eeg_analytical_solution", eeg_analytical_solution},
                    {"delete", delete_driver}};
    if (nrhs == 0) {
      mexErrMsgTxt("please provide a command");
      return;
    }
    auto cmd = commands.find(mxArrayToString(prhs[0]));
    if (cmd == commands.end()) {
      std::stringstream sstr;
      sstr << "command \"" << mxArrayToString(prhs[0]) << "\" not found";
      auto str = sstr.str();
      mexErrMsgTxt(str.c_str());
      return;
    } else {
      cmd->second(nlhs, plhs, nrhs - 1, prhs + 1);
    }
  }
}
