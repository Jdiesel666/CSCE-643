%% Main for Problem 3

close all; clear all; clc;

% Size of single image
img1 = imread('fig1.jpg');
img2 = imread('fig2.jpg');
T1 = load('T1.txt');
T2 = load('T2.txt');
[M,N,C] = size(img1);

Hhat = LMmethod;
fprintf('The optimal H hat is: \n');
display(Hhat);
H = T2^(-1) * Hhat * T1;

corner = transpose([1,1,1; N,1,1; 1,M,1; N,M,1]);
trans_corners = H * corner;
normx = [];
normy = [];
for i = 1:length(trans_corners)
    normx(i) = trans_corners(1, i) / trans_corners(3, i);
    normy(i) = trans_corners(2, i) / trans_corners(3, i);
end

xmin = round(min(normx));
xmax = round(max(normx)); 
ymin = round(min(normy)); 
ymax = round(max(normy));

img = zeros(ymax - ymin + 1,xmax - xmin + 1,C);     % Initialize mosaiced image 
img = mosaic(img, img1, H, xmin, ymin);             % Mosaicking img1 

img(2 - ymin : M + 1 - ymin, 2 - xmin : N + 1 - xmin, :) = img2;   % Mosaicing img2 

figure; 
imshow(img); 
imwrite(img,'pano.jpg');
