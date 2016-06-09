classdef duneuro_eeg < handle
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
        function solution = solve_direct(this,electrodes, dipoles)
            solution = duneuro_matlab('solve_direct', this.cpp_handle, electrodes, dipoles);
        end
    end
end
