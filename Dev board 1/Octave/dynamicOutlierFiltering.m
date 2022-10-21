function B = dynamicOutlierFiltering(data)
  for jj = 1:ncol
    A = data(:,jj)
    sig = std(abs(diff(A)));
    mu = mean(abs(diff(A)));

    for ii = 2:(numel(A)-1)
      if (abs(A(ii+1)-A(ii)))>(1.96*sig+mu) %if the difference is greater than the 95 percentile
        A_filt(ii) = (A(ii-1)+A(ii+1))/2;
      endif
    endfor
    B(jj) = A_filt; %may need to fix this later
   endfor
end

