classdef duneuro_meeg < handle
    properties (Hidden = true)
        cpp_handle;
    end
    methods
        % Constructor
        function this = duneuro_meeg(config)
            this.cpp_handle = duneuro_matlab('create', config);
        end
        % Destructor
        function delete(this)
            duneuro_matlab('delete', this.cpp_handle);
        end
        % write
        function solve_eeg_forward(this, dipole, func, config)
            duneuro_matlab('solve_eeg_forward', this.cpp_handle, dipole, func.cpp_handle, config);
        end
        function set_source_model(this, config)
            duneuro_matlab('set_source_model', this.cpp_handle, config);
        end
        function solution = solve_meg_forward(this, func, config)
            solution = duneuro_matlab('solve_meg_forward', this.cpp_handle, func.cpp_handle, config);
        end
        function matrix = compute_eeg_transfer_matrix(this, config)
            matrix = duneuro_matlab('compute_eeg_transfer_matrix', this.cpp_handle, config);
        end
        function matrix = compute_meg_transfer_matrix(this, config)
            matrix = duneuro_matlab('compute_meg_transfer_matrix', this.cpp_handle, config);
        end
        function set_electrodes(this, electrodes, config)
            duneuro_matlab('set_electrodes', this.cpp_handle, electrodes, config);
        end
        function set_coils_and_projections(this, coils, projections)
            duneuro_matlab('set_coils_and_projections', this.cpp_handle, coils, projections);
        end
        function solution = evaluate_at_electrodes(this, func)
            solution = duneuro_matlab('evaluate_at_electrodes', this.cpp_handle, func.cpp_handle);
        end
        function solution = apply_eeg_transfer(this, transfer_matrix, dipole, config)
            solution = duneuro_matlab('apply_eeg_transfer', this.cpp_handle, transfer_matrix, dipole, config);
        end
        function solution = apply_meg_transfer(this, transfer_matrix, dipole, config)
            solution = duneuro_matlab('apply_meg_transfer', this.cpp_handle, transfer_matrix, dipole, config);
        end
        function write(this, func, config)
            duneuro_matlab('write', this.cpp_handle, func.cpp_handle, config);
        end
    end
end
