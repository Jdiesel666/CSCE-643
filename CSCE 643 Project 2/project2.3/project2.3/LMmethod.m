function [H] = LMmethod()

%% Find the optimal Homography
% load data and construct matrices for L.M
in = load('X hat.txt');
dest = load('Xp hat.txt');

input = [in(1,:) ; in(2,:) ; dest(1,:) ; dest(2,:)];
want = zeros(length(input), 1);
hinit = reshape(load('H hat matrix.txt'), 9, 1);

opt = optimset('Algorithm','levenberg-marquardt', 'Tolfun', 1e-12); 

% Refined homography by L.M.
hnew = lsqcurvefit(@numeric, hinit, input, want, [], [], opt);      
H = reshape(hnew, 3, 3);
disp(H);
