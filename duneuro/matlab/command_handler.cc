#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <duneuro/matlab/command_handler.hh>

#include <duneuro/common/fitted_driver_data.hh>
#include <duneuro/driver/driver_factory.hh>
#include <duneuro/io/volume_conductor_vtk_writer.hh>
#include <duneuro/io/point_vtk_writer.hh>

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
    duneuro::MEEGDriverData<3> data;
    extract_fitted_driver_data_from_struct(prhs[0], data.fittedData);
    plhs[0] = convert_ptr_to_mat(
        DriverFactory<3>::make_driver(matlab_struct_to_parametertree(prhs[0]), data)
            .release());
    // note: mexLock has a lock count, call mexUnlock each time a driver is destroyed
    mexLock();
  }

  void CommandHandler::make_domain_function(int nlhs, mxArray* plhs[], int nrhs,
                                            const mxArray* prhs[])
  {
    if (nrhs != 1) {
      mexErrMsgTxt("one input required");
    }
    auto* foo = convert_mat_to_ptr<DriverInterface<3>>(prhs[0]);
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
    auto* foo = convert_mat_to_ptr<DriverInterface<3>>(prhs[0]);
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
    auto* foo = convert_mat_to_ptr<DriverInterface<3>>(prhs[0]);
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
    auto* foo = convert_mat_to_ptr<DriverInterface<3>>(prhs[0]);
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
    auto* foo = convert_mat_to_ptr<DriverInterface<3>>(prhs[0]);
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
    auto* foo = convert_mat_to_ptr<DriverInterface<3>>(prhs[0]);
    // the const cast below is a work around to fulfill the dense matrix interface.
    auto tm = extract_dense_matrix(const_cast<mxArray*>(prhs[1]));
    auto ae = foo->applyEEGTransfer(*tm, extract_dipoles(prhs[2]),
                                    matlab_struct_to_parametertree(prhs[3]));
    plhs[0] = mxCreateDoubleMatrix(tm->rows(), ae.size(), mxREAL);
    auto pr = mxGetPr(plhs[0]);
    for (const auto& a : ae) {
      std::copy(a.begin(), a.end(), pr);
      pr += a.size();
    }
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
    auto* foo = convert_mat_to_ptr<DriverInterface<3>>(prhs[0]);
    // the const cast below is a work around to fulfill the dense matrix interface.
    auto tm = extract_dense_matrix(const_cast<mxArray*>(prhs[1]));
    auto ae = foo->applyMEGTransfer(*tm, extract_dipoles(prhs[2]),
                                    matlab_struct_to_parametertree(prhs[3]));
    plhs[0] = mxCreateDoubleMatrix(tm->rows(), ae.size(), mxREAL);
    auto pr = mxGetPr(plhs[0]);
    for (const auto& a : ae) {
      std::copy(a.begin(), a.end(), pr);
      pr += a.size();
    }
  }

  void CommandHandler::get_projected_electrodes(int nlhs, mxArray* plhs[], int nrhs,
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
    auto* foo = convert_mat_to_ptr<DriverInterface<3>>(prhs[0]);
    auto electrodes = foo->getProjectedElectrodes();
    plhs[0] = mxCreateDoubleMatrix(3, electrodes.size(), mxREAL);
    auto* pr = mxGetPr(plhs[0]);
    for (unsigned int i = 0; i < electrodes.size(); ++i, pr += 3) {
      std::copy(electrodes[i].begin(), electrodes[i].end(), pr);
    }
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
    auto* foo = convert_mat_to_ptr<DriverInterface<3>>(prhs[0]);
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
    auto* foo = convert_mat_to_ptr<DriverInterface<3>>(prhs[0]);
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
    auto* foo = convert_mat_to_ptr<DriverInterface<3>>(prhs[0]);
    auto* sol = convert_mat_to_ptr<Function>(prhs[1]);
    auto ae = foo->evaluateAtElectrodes(*sol);
    plhs[0] = mxCreateDoubleMatrix(ae.size(), 1, mxREAL);
    std::copy(ae.begin(), ae.end(), mxGetPr(plhs[0]));
  }

  void CommandHandler::print_citations(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
  {
    if (nlhs != 0) {
      mexErrMsgTxt("the method does not return variables");
      return;
    }
    if (nrhs == 1) {
        auto* foo = convert_mat_to_ptr<DriverInterface<3>>(prhs[0]);
        foo->print_citations();
    }
    else {
      mexErrMsgTxt("please provide a handle to the object");
    }
  }

  void CommandHandler::delete_driver(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
  {
    if (nrhs == 0) {
      mexErrMsgTxt("please provide a handle to the object");
      return;
    }
    auto* foo = convert_mat_to_ptr<DriverInterface<3>>(prhs[0]);
    delete foo;
    mexUnlock();
  }

  /**********************************************
   * functions related to visualization
   **********************************************/
  void CommandHandler::volume_conductor_vtk_writer(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
  {
    if (nlhs != 1) {
      mexErrMsgTxt("expected exactly one output argument");
      return;
    }
    if (nrhs != 2) {
      mexErrMsgTxt("expected 2 arguments (driver and config), but got different number of arguments");
      return;
    }
    
    auto* driver_ptr = convert_mat_to_ptr<DriverInterface<3>>(prhs[0]);
    std::unique_ptr<VolumeConductorVTKWriterInterface> writer_ptr = driver_ptr->volumeConductorVTKWriter(matlab_struct_to_parametertree(prhs[1]));
    plhs[0] = convert_ptr_to_mat(writer_ptr.release());
    mexLock();
  }
  
  void CommandHandler::volume_writer_add_vertex_data(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
  {
    if(nrhs != 3) {
      mexErrMsgTxt("wrong number of input arguments (please supply writer, function, and output name)");
      return;
    }
    if(nlhs != 0) {
      mexErrMsgTxt("Wrong number of output arguments");
      return;
    }
    if(!mxIsChar(prhs[2])) {
      mexErrMsgTxt("last argument needs to be a char array");
      return;
    }
    
    auto* writer_ptr = convert_mat_to_ptr<VolumeConductorVTKWriterInterface>(prhs[0]);
    auto* function_ptr = convert_mat_to_ptr<Function>(prhs[1]);
    writer_ptr->addVertexData(*function_ptr, std::string(mxArrayToString(prhs[2])));
  }
  
  void CommandHandler::volume_writer_add_vertex_data_gradient(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
  {
    if(nrhs != 3) {
      mexErrMsgTxt("wrong number of input arguments (please supply writer, function, and output name)");
      return;
    }
    if(nlhs != 0) {
      mexErrMsgTxt("Wrong number of output arguments");
      return;
    }
    if(!mxIsChar(prhs[2])) {
      mexErrMsgTxt("last argument needs to be a char array");
      return;
    }
    
    auto* writer_ptr = convert_mat_to_ptr<VolumeConductorVTKWriterInterface>(prhs[0]);
    auto* function_ptr = convert_mat_to_ptr<Function>(prhs[1]);
    writer_ptr->addVertexDataGradient(*function_ptr, std::string(mxArrayToString(prhs[2])));
  }
  
  void CommandHandler::volume_writer_add_cell_data(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
  {
    if(nrhs != 3) {
      mexErrMsgTxt("wrong number of input arguments (please supply writer, function, and output name)");
      return;
    }
    if(nlhs != 0) {
      mexErrMsgTxt("Wrong number of output arguments");
      return;
    }
    if(!mxIsChar(prhs[2])) {
      mexErrMsgTxt("last argument needs to be a char array");
      return;
    }
    
    auto* writer_ptr = convert_mat_to_ptr<VolumeConductorVTKWriterInterface>(prhs[0]);
    auto* function_ptr = convert_mat_to_ptr<Function>(prhs[1]);
    writer_ptr->addCellData(*function_ptr, std::string(mxArrayToString(prhs[2])));
  }
  
  void CommandHandler::volume_writer_add_cell_data_gradient(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
  {
    if(nrhs != 3) {
      mexErrMsgTxt("wrong number of input arguments (please supply writer, function, and output name)");
      return;
    }
    if(nlhs != 0) {
      mexErrMsgTxt("Wrong number of output arguments");
      return;
    }
    if(!mxIsChar(prhs[2])) {
      mexErrMsgTxt("last argument needs to be a char array");
      return;
    }
    
    auto* writer_ptr = convert_mat_to_ptr<VolumeConductorVTKWriterInterface>(prhs[0]);
    auto* function_ptr = convert_mat_to_ptr<Function>(prhs[1]);
    writer_ptr->addCellDataGradient(*function_ptr, std::string(mxArrayToString(prhs[2])));
  }
  
  void CommandHandler::volume_writer_write(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
  {
    if(nrhs != 2 || nlhs != 0) {
      mexErrMsgTxt("wrong number of arguments (please provide writer and configuration struct)");
      return;
    }
    if(nlhs != 0) {
      mexErrMsgTxt("Wrong number of output arguments");
      return;
    }
    
    auto* writer_ptr = convert_mat_to_ptr<VolumeConductorVTKWriterInterface>(prhs[0]);
    writer_ptr->write(matlab_struct_to_parametertree(prhs[1]));
  }
  
  void CommandHandler::volume_writer_delete(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
  {
    if(nrhs != 1 || nlhs != 0) {
      mexErrMsgTxt("wrong number of input or output arguments");
      return;
    } 
    
    auto* writer_ptr = convert_mat_to_ptr<VolumeConductorVTKWriterInterface>(prhs[0]);
    delete writer_ptr;
    mexUnlock();
  }

  void CommandHandler::point_vtk_writer(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
  {
    if(nrhs != 1) {
      mexErrMsgTxt("wrong number of input arguments (please provide either a 3xN double matrix representing points or a 6x1 matrix representing a dipole, interpreted as (pos, moment))");
      return;
    }
    const mxArray* data = prhs[0];
    if(!mxIsDouble(data)) {
      mexErrMsgTxt("expected double matrix");
      return;
    }
    
    int rows = mxGetM(data);
    int cols = mxGetM(data);
    
    if(!(rows == 3 || mxGetNumberOfElements(data) == 6)) {
      mexErrMsgTxt("expected point matrix or single dipole");
      return;
    }
    
    PointVTKWriter<double, 3>* point_writer;
    
    if(rows == 3) {
      auto points = extract_field_vectors(data);
      point_writer = new PointVTKWriter<double, 3>(points);
    }
    // dipole case
    else {
      auto dipole = extract_dipole(data);
      point_writer = new PointVTKWriter<double, 3>(dipole);
    }
    
    plhs[0] = convert_ptr_to_mat(point_writer);
    mexLock();
  }

  void CommandHandler::point_writer_add_scalar_data(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
  {
    if(nrhs != 3) {
      mexErrMsgTxt("wrong number of input arguments, please provide writer, scalar data array, and output name");
    }
    
    auto* writer_ptr = convert_mat_to_ptr<PointVTKWriter<double, 3>>(prhs[0]);
    const mxArray* data = prhs[1];
    int rows = mxGetM(data);
    int cols = mxGetN(data);
    if(rows != 1 && cols != 1) {
      mexErrMsgTxt("input error, please provide scalar data");
      return;
    }
    if(!mxIsChar(prhs[2])) {
      mexErrMsgTxt("last argument need to be a char array");
      return;
    }
    
    writer_ptr->addScalarData(std::string(mxArrayToString(prhs[2])), extract_vector(prhs[1]));
  }
  
  void CommandHandler::point_writer_add_vector_data(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
  {
    if(nrhs != 3) {
      mexErrMsgTxt("wrong number of input arguments (please provide writer, vector data array, and output name)");
    }
    
    auto* writer_ptr = convert_mat_to_ptr<PointVTKWriter<double, 3>>(prhs[0]);
    const mxArray* data = prhs[1];
    int rows = mxGetM(data);
    int cols = mxGetN(data);
    if(rows != 3) {
      mexErrMsgTxt("input error, please provide vector data as 3xN array");
      return;
    }
    if(!mxIsChar(prhs[2])) {
      mexErrMsgTxt("last argument need to be a char array");
      return;
    }
    
    writer_ptr->addVectorData(std::string(mxArrayToString(prhs[2])), extract_field_vectors(prhs[1]));
  }

  void CommandHandler::point_writer_write(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
  {
    if(nrhs != 2) {
      mexErrMsgTxt("wrong number of input arguments (please provide writer and filename for output)");
      return;
    }
    if(!mxIsChar(prhs[1])) {
      mexErrMsgTxt("last argument need to be a char array");
      return;
    }
    
    auto* writer_ptr = convert_mat_to_ptr<PointVTKWriter<double, 3>>(prhs[0]);
    writer_ptr->write(std::string(mxArrayToString(prhs[1])));
  }

  void CommandHandler::point_writer_delete(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
  {
    if(nrhs != 1 || nlhs != 0) {
      mexErrMsgTxt("wrong number of input or output arguments");
      return;
    }
    
    auto* point_writer_ptr = convert_mat_to_ptr<PointVTKWriter<double, 3>>(prhs[0]);
    delete point_writer_ptr;
    mexUnlock();
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
                    {"get_projected_electrodes", get_projected_electrodes},
                    {"set_coils_and_projections", set_coils_and_projections},
                    {"evaluate_at_electrodes", evaluate_at_electrodes},
                    {"print_citations", print_citations},
                    {"delete", delete_driver},
                    
                    {"volume_conductor_vtk_writer", volume_conductor_vtk_writer},
                    {"volume_writer_add_vertex_data", volume_writer_add_vertex_data},
                    {"volume_writer_add_vertex_data_gradient", volume_writer_add_vertex_data_gradient},
                    {"volume_writer_add_cell_data", volume_writer_add_cell_data},
                    {"volume_writer_add_cell_data_gradient", volume_writer_add_cell_data_gradient},
                    {"volume_writer_write", volume_writer_write},
                    {"volume_writer_delete", volume_writer_delete},
                    
                    {"point_vtk_writer", point_vtk_writer},
                    {"point_writer_add_scalar_data", point_writer_add_scalar_data},
                    {"point_writer_add_vector_data", point_writer_add_vector_data},
                    {"point_writer_write", point_writer_write},
                    {"point_writer_delete", point_writer_delete}};
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
