function [Y,F] = generateFFTspectrum(t,y)
  Y = abs(fft(y));
  f_samp = 1/mean(diff(t));

  L = numel(y);
  F = f_samp/L*(0:(L/2))
  Y = Y(1:numel(F))/L;


end























