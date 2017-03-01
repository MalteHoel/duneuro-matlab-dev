function y = duneuro_analytical(dipole, electrodes, config)

y = duneuro_matlab('analytical_eeg_forward', dipole, electrodes, config);
