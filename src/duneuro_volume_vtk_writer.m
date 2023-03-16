classdef duneuro_volume_vtk_writer < handle
    properties  (Hidden = true)
      cpp_handle;
    end

    methods
      % constructor
      function this = duneuro_volume_vtk_writer(driver, config)
        this.cpp_handle = duneuro_matlab('volume_conductor_vtk_writer', driver.cpp_handle, config);
      end
      
      % functions for registering data
      function add_vertex_data(this, duneuro_function_instance, name)
        duneuro_matlab('volume_writer_add_vertex_data', this.cpp_handle, duneuro_function_instance.cpp_handle, name);
      end
      
      function add_vertex_data_gradient(this, duneuro_function_instance, name)
        duneuro_matlab('volume_writer_add_vertex_data_gradient', this.cpp_handle, duneuro_function_instance.cpp_handle, name);
      end
      
      function add_cell_data(this, duneuro_function_instance, name)
        duneuro_matlab('volume_writer_add_cell_data', this.cpp_handle, duneuro_function_instance.cpp_handle, name);
      end
      
      function add_cell_data_gradient(this, duneuro_function_instance, name)
        duneuro_matlab('volume_writer_add_cell_data_gradient', this.cpp_handle, duneuro_function_instance.cpp_handle, name);
      end
      
      % write volume conductor and registered data to file
      function write(this, config)
        duneuro_matlab('volume_writer_write', this.cpp_handle, config);
      end
      
      % destructor
      function delete(this)
        duneuro_matlab('volume_writer_delete', this.cpp_handle);
      end
    end
end
