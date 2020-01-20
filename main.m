load("maze.mat");
load('pool.mat');

maze = I;
maze_rotated = imrotate(maze, 45);

F = double(I);
F_rotated = imrotate(double(I), 45);

F_wall_indicators = F == 0;
F(F_wall_indicators) = 100;

F_rotated_wall_indicators = F_rotated == 0;
F_rotated(F_rotated_wall_indicators) = 100;

% Solve maze
source = [815, 384];
target = [9, 234];
maze_dist = fmm(F, source, [0], 'silent');

figure;
imagesc(maze_dist);
title("Distance Map (Maze)");

figure;
path = shortest_path(maze_dist, source, target);
imshow(maze);
hold on
plot_path(path, 'blue');
hold off
title("Maze Solution");

% Solve roated maze
source = [849, 737];
target = [188, 1214];
maze45_dist = fmm(F_rotated, source, [0], 'silent');
figure;
imagesc(maze45_dist);
title("Distance Map (Rotated Maze)");

figure;
path = shortest_path(maze45_dist, source, target);
imshow(maze_rotated);
hold on
plot_path(path, 'blue');
hold off
title("Rotated Maze Solution");

% Solve optical path
source = [399, 499];
target = [1, 1];
T = fmm(double(n), source, [0], 'silent');
path = shortest_path(T, source, target);

figure;
imagesc(n);
hold on;
plot_path(path, 'red');
hold off;
title("Optical Path");
