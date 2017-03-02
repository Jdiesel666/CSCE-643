%% Calculation for Jacobian and Epsilon
function [D] = numeric(hinit, input)

%% load data
input1 = load('A matrix.txt');
input2 = load('H hat matrix.txt');
input3 = load('X hat.txt');
input4 = load('Xp hat.txt');

%% initialization
A = input1;
hinit = reshape(input2, 9, 1);
xi = [transpose(input3(1, :)), transpose(input3(2, :)),... 
    transpose(input3(3, :))];
xp = transpose(input4(1, :));
yp = transpose(input4(2, :));

%% calculations
eps = A * hinit;    % find epsilon

J = [];             % find jacobian
for ct = 1:length(input3)
    % find each J element for each point
    J11 = -hinit(4) + yp(ct) * hinit(7);
    J21 = hinit(1) - xp(ct) * hinit(7);
    J12 = -hinit(5) + yp(ct) * hinit(8);
    J22 = hinit(2) - xp(ct) * hinit(8);
    J13 = 0;
    J23 = - xi(ct, :) * [hinit(7); hinit(8); hinit(9)];
    J14 = xi(ct, :) * [hinit(7); hinit(8); hinit(9)];
    J24 = 0;
    % build J matrix
    J(2 * ct - 1, 1) = J11; 
    J(2 * ct - 1, 2) = J12; 
    J(2 * ct - 1, 3) = J13; 
    J(2 * ct - 1, 4) = J14; 
    J(2 * ct, 1) = J21;
    J(2 * ct, 2) = J22;
    J(2 * ct, 3) = J23;
    J(2 * ct, 4) = J24;
    
end

D = [];          % find error
for n = 1:length(input3)
    
    Jitrans = transpose([J(2 * n - 1, :); J(2 * n, :)]);    % Ji^t
    JJi = inv([J(2 * n - 1, :); J(2 * n, :)] * Jitrans);    % (JJi^t)^(-1)
    epsi = [eps(2 * n - 1); eps(2 * n)];
    Di = transpose(epsi) * JJi * epsi;                      % calc each D
    
    D(n) = sqrt(Di);
end

D = transpose(D);   % finalizing D matrix
