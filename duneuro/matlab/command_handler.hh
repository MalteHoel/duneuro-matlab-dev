#include <mex.h>

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
    static void apply_transfer(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);
    /** \TODO docme! */
    static void set_electrodes(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);
    /** \TODO docme! */
    static void set_coils_and_projections(int nlhs, mxArray* plhs[], int nrhs,
                                          const mxArray* prhs[]);
    /** \TODO docme! */
    static void evaluate_at_electrodes(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);
    /** \TODO docme! */
    static void write(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);
    /** \TODO docme! */
    static void delete_driver(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);
    /** \TODO docme! */
    static void run_command(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);
  };
}
