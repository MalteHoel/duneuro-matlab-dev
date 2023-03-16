classdef duneuro_point_vtk_writer < handle
  properties (Hidden = true)
    cpp_handle;
  end

  methods
    % constructor
    function this = duneuro_point_vtk_writer(point_data_or_dipole)
      this.cpp_handle = duneuro_matlab('point_vtk_writer', point_data_or_dipole);
    end
    
    function add_scalar_data(this, point_data, name)
      duneuro_matlab('point_writer_add_scalar_data', this.cpp_handle, point_data, name);
    end
    
    function add_vector_data(this, vector_data, name)
      duneuro_matlab('point_writer_add_vector_data', this.cpp_handle, vector_data, name);
    end
    
    function write(this, filename)
      duneuro_matlab('point_writer_write', this.cpp_handle, filename);
    end
    
    % destructor
    function delete(this)
      duneuro_matlab('point_writer_delete', this.cpp_handle);
    end
  end
end
