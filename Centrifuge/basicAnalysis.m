clear all
close all
clc

data = csvread('test1-4-27-24-2308.csv');

t = data(2:end,1)/1000000;
accelx = data(2:end,2)/9.81;

[Y,F] = generateFFTspectrum(t,accelx);

plot(F,Y)















