%% quaternion multiplication tool
% assumes quaternions are in order [mag,i,j,k]
function q3 = quatMult(q1,q2)
  if numel(q1~=4)||numel(q2~=4)
    error("one of the inputs is not in quaternion format")
  endif

  %multiply all q2 by each q1 value
  r1 = q1(1)*q2; %[mag,i,j,k]
  r2 = q1(2)*q2; %[i,i*i,i*j,i*k]
  r3 = q1(3)*q2; %[j,j*i,j*j,j*k]
  r4 = q1(4)*q2; %[k,k*i,k*j,k*k]

  %correct for imaginary axes
  r2 = [-r2(2),r2(1),-r2(4),r2(3)]; %[mag,i,j,k]
  r3 = [-r3(3),r3(4),r3(1),-r3(2)]; %[mag,i,j,k]
  r4 = [-r4(4),-r4(3),r4(2),r4(1)]; %[mag,i,j,k]

  %add results to form final quaternion
  q3 = r1+r2+r3+r4;
end


