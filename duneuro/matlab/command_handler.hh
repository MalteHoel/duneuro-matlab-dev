#ifndef DUNEURO_MATLAB_COMMAND_HANDLER_HH
#define DUNEURO_MATLAB_COMMAND_HANDLER_HH

#include <mex.h>
#include <memory>

namespace duneuro
{
  struct CommandHandler {
    /** \TODO docme! */
    static void create_driver(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);
    /** \TODO docme! */
    static void make_domain_function(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);
    /** \TODO docme! */
    static void delete_function(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);
    /** \TODO docme! */
    static void solve_eeg_forward(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);
    /** \TODO docme! */
    static void solve_meg_forward(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);
    /** \TODO docme! */
    static void compute_eeg_transfer_matrix(int nlhs, mxArray* plhs[], int nrhs,
                                            const mxArray* prhs[]);
    /** \TODO docme! */
    static void compute_meg_transfer_matrix(int nlhs, mxArray* plhs[], int nrhs,
                                            const mxArray* prhs[]);
    /** \TODO docme! */
    static void apply_eeg_transfer(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);
    /** \TODO docme! */
    static void apply_meg_transfer(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);
    /** \TODO docme! */
    static void set_electrodes(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);
    /** \TODO docme! */
    static void get_projected_electrodes(int nlhs, mxArray* plhs[], int nrhs,
                                         const mxArray* prhs[]);
    /** \TODO docme! */
    static void set_coils_and_projections(int nlhs, mxArray* plhs[], int nrhs,
                                          const mxArray* prhs[]);
    /** \TODO docme! */
    static void evaluate_at_electrodes(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);
    /** \TODO docme! */
    static void write(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);
    /** \TODO docme! */
    static void print_citations(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);
    /** \TODO docme! */
    static void delete_driver(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);
   
    // visualization functionality
    static void volume_conductor_vtk_writer(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);
    static void volume_writer_add_vertex_data(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);
    static void volume_writer_add_vertex_data_gradient(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);
    static void volume_writer_add_cell_data(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);
    static void volume_writer_add_cell_data_gradient(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);
    static void volume_writer_write(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);
    static void volume_writer_delete(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);
    
    static void point_vtk_writer(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);
    static void point_writer_add_scalar_data(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);
    static void point_writer_add_vector_data(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);
    static void point_writer_write(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);
    static void point_writer_delete(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);
    
    /** \TODO docme! */
    static void run_command(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);
  };
}

#endif // DUNEURO_MATLAB_COMMAND_HANDLER_HH
