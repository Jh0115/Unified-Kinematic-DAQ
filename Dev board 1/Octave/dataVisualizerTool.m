%% Simple data visualization tool for UKDAQ demo board
clear all
close all
clc

%% Step 1: Unpack the CSV
data = csvread('DATA.csv'); %change filename here to match whatever the correct name is

%drop the first line of data, its probably bad
data = data([2:end],:);

%seperate relevant sensor packages
t = data(:,1); %time in milliseconds
accel = data(:,[2,3,4]); %acceleration vector in m/s^2
quat = data(:,[5,6,7,8]); %quaternion orientation data in... some units...
temp = data(:,9); %temperature in deg C
press = data(:,10); %pressure in hPa
alt = data(:,11); %altitude from pressure in m

%% Step 2: Filter outliers
accel = dynamicOutlierFiltering(accel);
quat = dynamicOutlierFiltering(quat);
temp = dynamicOutlierFiltering(temp);
press = dynamicOutlierFiltering(press);
alt = dynamicOutlierFiltering(alt);

%% Step 3: Convert body frame vectors to inertial frame
quat_conj = [quat(:,1),-quat(:,2),-quat(:,3),-quat(:,4)];

for ii = 1:numel(t)
  quat_accel = [0,accel(ii,1),accel(ii,2),accel(ii,3)];
  q1 = quatMult(quat(ii,:),quat_accel);
  accel_new(ii) = quatMult(q1,quat_conj(ii,:));
endfor
accel = accel_new;

%% Step 4: Integrate acceleration to position-velocity vectors
t = t/1000; %convert time to seconds

vel = trapz(t,accel) %m/s
pos = trapz(t,vel) %m


%% Step 4: Calculate descent rate from combined altitude data
DR = diff(alt)./diff(t);

%% Step 5: Play with data a little bit and determine when the user jumps out of the plane
for ii = 1:numel(t)
  Gs(ii) = norm(accel(ii,:))/9.81; %get the G count over the entire dataset
  speed(ii) = norm(vel(ii,:)); %the overall speed
endfor

t_fall_assum = 900; %assume freefall and parachute and data off takes 15 minutes
% anytime in this period the jumper could jump and begin freefall

avg_t_diff = mean(diff(t));
t_ind_fall_assum = round(t_fall_assum/avg_t_diff);

t_fall_watch = t(t_ind_fall_assum:end)

t_fall = find(abs(DR(t_fall_watch))>17.5)
t_jump = t(t_fall(1));

n = t_fall(1);
flag = False;
while (flag==False)
  if abs(DR(n))<10
    flag = True;
    t_pull_ind = n;
  endif
  n = n+1;
endwhile

t_ind_landing = find((alt(end/2:end)-alt(1))<5); %keep track of times when altitude is 5 meters above ground level after half of the data
v_horiz = norm([vel(:,1).vel(:,2)])
landingSpeed = v_horiz(t_ind_landing);

%% Step 6: Grab interesting data points
maxDR = max(DR);
maxGs = max(Gs);
jumpAlt = max(alt);
maxSpeed = max(speed);
deploymentOffset = norm([pos(t_jump,1),pos(t_jump,2)]);
pullAlt = alt(t_pull_ind)-alt(end);
maxLandSpeed = max(landingSpeed);



