classdef duneuro_meeg < handle
    properties (Hidden = true, SetAccess = private)
        cpp_handle;
    end
    methods
        % Constructor
        function this = duneuro_eeg(config)
            this.cpp_handle = duneuro_matlab('create', config);
        end
        % Destructor
        function delete(this)
            duneuro_matlab('delete', this.cpp_handle);
        end
        % write
        function solution = solve_eeg_forward(this,electrodes, dipoles)
            solution = duneuro_matlab('solve_eeg_forward', this.cpp_handle, electrodes, dipoles);
        end
        function matrix = compute_eeg_transfer_matrix(this,electrodes)
            matrix = duneuro_matlab('compute_eeg_transfer_matrix', this.cpp_handle, electrodes);
        end
        function solution = apply_transfer(this,transfer_matrix, dipoles)
            solution = duneuro_matlab('apply_transfer', this.cpp_handle, transfer_matrix, dipoles);
        end
    end
end
