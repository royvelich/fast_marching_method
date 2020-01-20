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

source = [815, 384];
target = [9, 234];

% T = dist_mex(F, source, [0], 'silent');
% path = shortest_path(T, source, target);

% figure;
% imshow(F);
% hold on;
% plot_path(path, 'blue');
% hold off;


% F_copy = F;
% F_copy(815, 384) = 123;
% F_copy_rotated = imrotate(F_copy, 45);
% [rows, cols] = size(F_copy_rotated);
% for row = 1 : rows
%     for col = 1 : cols
%         if F_copy_rotated(row, col) == 123
%             source_rotated = [col, row];
%         end
%     end
% end


source = [849, 737];
target = [188, 1214];

T = dist_mex(F_rotated, source, [0], 'silent');
path = shortest_path(T, source, target);

figure;
imshow(maze_rotated);
hold on;
plot_path(path, 'blue');
hold off;

