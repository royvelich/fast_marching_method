

load("maze.mat");
load('pool.mat');
F = double(I);
wall_indicators = F == 0;
F(wall_indicators) = 100;
dist = dist_mex(F, [815, 384], [0], 'silent');

% imshow(I);
% F = double(I);


% mymap = [0 0 0
%          1 0 0
%          0 1 0];
%      
% mat = [0,1,2;2,1,0;1,1,1];
% colormap(mymap)
% imagesc(mat)
% 
% 
% 
% 
% A = [1,2,3,4,5];
% B = [4,5,6,7,8];
% 
% C = cat(3, A, B);
% 
% x = [1,2];
% path = x;
% path(end + 1, :) = [3,4];
% 
% bla = vecnorm(C, 2, 3);
% 
% D = repmat(vecnorm(C, 2, 3), [1 1 2]);

% load('pool.mat');

% dist = dist_mex(F, [815, 384], [0], 'silent');
% bla = dist .* F;
% 
% N = normalize(dist)
% imagesc(bla)