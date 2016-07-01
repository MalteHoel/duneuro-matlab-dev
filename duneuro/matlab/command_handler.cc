#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <duneuro/matlab/command_handler.hh>

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
    mexLock();
    auto config = matlab_struct_to_parametertree(prhs[0]);
    plhs[0] = convert_ptr_to_mat(MEEGDriverFactory::make_meeg_driver(config).release());
    std::cout << "created and lock called\n";
  }

  void CommandHandler::make_domain_function(int nlhs, mxArray* plhs[], int nrhs,
                                            const mxArray* prhs[])
  {
    if (nrhs != 1) {
      mexErrMsgTxt("one input required");
    }
    auto* foo = convert_mat_to_ptr<MEEGDriverInterface>(prhs[0]);
    plhs[0] = convert_ptr_to_mat(new Function(foo->makeDomainFunction()));
  }

  void CommandHandler::delete_function(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
  {
    if (nrhs != 1) {
      mexErrMsgTxt("please provide a function handle");
      return;
    }
    auto* foo = convert_mat_to_ptr<Function>(prhs[0]);
    delete foo;
  }

  void CommandHandler::solve_eeg_forward(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
  {
    if (nrhs < 3) {
      mexErrMsgTxt("please provide a handle to the object, the dipole and the solution function");
      return;
    }
    if (nlhs != 0) {
      mexErrMsgTxt("the method does not return variables");
      return;
    }
    auto* foo = convert_mat_to_ptr<MEEGDriverInterface>(prhs[0]);
    auto* solution = convert_mat_to_ptr<Function>(prhs[2]);
    auto dipole = extract_dipole(prhs[1]);
    std::cout << "solving for dipole at " << dipole.position() << ", " << dipole.moment() << "\n";
    foo->solveEEGForward(dipole, *solution);
  }

  void CommandHandler::solve_meg_forward(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
  {
    if (nrhs < 2) {
      mexErrMsgTxt("please provide a handle to the object and a handle to the eeg solution");
      return;
    }
    if (nlhs != 1) {
      mexErrMsgTxt("the method returns a matrix");
      return;
    }
    auto* foo = convert_mat_to_ptr<MEEGDriverInterface>(prhs[0]);
    auto* sol = convert_mat_to_ptr<Function>(prhs[1]);
    auto ae = foo->solveMEGForward(*sol);
    plhs[0] = mxCreateDoubleMatrix(ae.size(), 1, mxREAL);
    std::copy(ae.begin(), ae.end(), mxGetPr(plhs[0]));
  }

  void CommandHandler::compute_eeg_transfer_matrix(int nlhs, mxArray* plhs[], int nrhs,
                                                   const mxArray* prhs[])
  {
    if (nrhs < 1) {
      mexErrMsgTxt("please provide a handle to the object");
      return;
    }
    if (nlhs != 1) {
      mexErrMsgTxt("the method returns a matrix");
      return;
    }
    auto* foo = convert_mat_to_ptr<MEEGDriverInterface>(prhs[0]);
    auto tm = foo->computeEEGTransferMatrix();
    plhs[0] = mxCreateDoubleMatrix(tm->cols(), tm->rows(), mxREAL);
    std::copy(tm->data(), tm->data() + tm->rows() * tm->cols(), mxGetPr(plhs[0]));
  }

  void CommandHandler::compute_meg_transfer_matrix(int nlhs, mxArray* plhs[], int nrhs,
                                                   const mxArray* prhs[])
  {
    if (nrhs < 1) {
      mexErrMsgTxt("please provide a handle to the object");
      return;
    }
    if (nlhs != 1) {
      mexErrMsgTxt("the method returns a matrix");
      return;
    }
    auto* foo = convert_mat_to_ptr<MEEGDriverInterface>(prhs[0]);
    auto tm = foo->computeMEGTransferMatrix();
    plhs[0] = mxCreateDoubleMatrix(tm->cols(), tm->rows(), mxREAL);
    std::copy(tm->data(), tm->data() + tm->rows() * tm->cols(), mxGetPr(plhs[0]));
  }

  void CommandHandler::apply_transfer(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
  {
    if (nrhs < 3) {
      mexErrMsgTxt("please provide a handle to the object, the transfer matrix and the dipole");
      return;
    }
    if (nlhs != 1) {
      mexErrMsgTxt("the method returns a matrix");
      return;
    }
    auto* foo = convert_mat_to_ptr<MEEGDriverInterface>(prhs[0]);
    // the const cast below is a work around to fulfill the dense matrix interface.
    auto tm = extract_dense_matrix(const_cast<mxArray*>(prhs[1]));
    auto dipole = extract_dipole(prhs[2]);
    auto ae = foo->applyTransfer(*tm, dipole);
    plhs[0] = mxCreateDoubleMatrix(tm->rows(), 1, mxREAL);
    std::copy(ae.begin(), ae.end(), mxGetPr(plhs[0]));
  }

  void CommandHandler::set_electrodes(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
  {
    if (nrhs < 2) {
      mexErrMsgTxt("please provide a handle to the object and the electrodes");
      return;
    }
    if (nlhs != 0) {
      mexErrMsgTxt("the method does not return variables");
      return;
    }
    auto* foo = convert_mat_to_ptr<MEEGDriverInterface>(prhs[0]);
    foo->setElectrodes(extract_field_vectors(prhs[1]));
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
    auto* foo = convert_mat_to_ptr<MEEGDriverInterface>(prhs[0]);
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
    auto* foo = convert_mat_to_ptr<MEEGDriverInterface>(prhs[0]);
    auto* sol = convert_mat_to_ptr<Function>(prhs[1]);
    auto ae = foo->evaluateAtElectrodes(*sol);
    plhs[0] = mxCreateDoubleMatrix(ae.size(), 1, mxREAL);
    std::copy(ae.begin(), ae.end(), mxGetPr(plhs[0]));
  }

  void CommandHandler::write(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
  {
    if (nrhs < 3) {
      mexErrMsgTxt(
          "please provide a handle to the object, the config and a handle to the function");
      return;
    }
    if (nlhs != 0) {
      mexErrMsgTxt("the method does not return variables");
      return;
    }
    auto* foo = convert_mat_to_ptr<MEEGDriverInterface>(prhs[0]);
    auto ptree = matlab_struct_to_parametertree(prhs[1]);
    auto* sol = convert_mat_to_ptr<Function>(prhs[2]);
    foo->write(ptree, *sol);
  }

  void CommandHandler::delete_driver(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
  {
    if (nrhs == 0) {
      mexErrMsgTxt("please provide a handle to the object");
      return;
    }
    auto* foo = convert_mat_to_ptr<MEEGDriverInterface>(prhs[0]);
    delete foo;
    mexUnlock();
    std::cout << "deleted and unlock called\n";
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
                    {"apply_transfer", apply_transfer},
                    {"set_electrodes", set_electrodes},
                    {"set_coils_and_projections", set_coils_and_projections},
                    {"evaluate_at_electrodes", evaluate_at_electrodes},
                    {"write", write},
                    {"delete", delete_driver}};
    if (nrhs == 0) {
      mexErrMsgTxt("please provide a command");
      return;
    }
    auto cmd = commands.find(mxArrayToString(prhs[0]));
    if (cmd == commands.end()) {
      mexErrMsgTxt("command not found");
      return;
    } else {
      cmd->second(nlhs, plhs, nrhs - 1, prhs + 1);
    }
  }
}
