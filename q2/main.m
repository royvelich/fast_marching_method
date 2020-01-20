% Number of grid samples
n = 300;

% Sample 2D grid
grid = linspace(-5, 5, n);

% Get coordinates of grid points
[X, Y] = meshgrid(grid);

% Generate two paraboloids shifted from each other along the X axis. Add a
% vertical negative offset so we can slice the surface at z = 0
paraboloid1 = (X - 2) .^ 2 + Y .^ 2 - 5;
paraboloid2 = (X + 2) .^ 2 + Y .^ 2 - 5;

Z = min(paraboloid1, paraboloid2);
% Z = reshape(Z, n, n);

dt = 1e-1;
z0=0;
plot_contour_line(grid, X, Y, Z);
plot_every = 100;
for i = 1 : 700
    % Approximate surface normal (implicit function gradient) using finite
    % differences
    [Zx, Zy] = gradient(Z);
    
    % Concatenate partial derivatives
    Zg = cat(3, Zx, Zy);
    
    % Calculate gradient norm at each grid point
    g_norm = vecnorm(Zg, 2, 3);
    
    % Evolve Z by infinitesimal delta of gradient norm
    Z = Z + dt * g_norm;
    
    % Plot curve evolution every few iterations
    if mod(i, plot_every) == 0
        plot_contour_line(grid, X, Y, Z);
    end
end
    
function k = curvature(h, Z)
    [Zx, Zy] = gradient(Z, h);
    [Zxx, Zxy] = gradient(Zx, h);
    [~, Zyy] = gradient(Zy, h);
    Zx2 = Zx .^ 2;
    Zy2 = Zy .^ 2;
    k = - (Zxx .* Zy2 - 2 * Zx .* Zy .* Zxy + Zyy .* Zx2) ./ ((Zx2 + Zy2) .^ (3/2));
end
% 
% function [s, k] = curve_s_k(space, X, Y, Z, z0)
%     M = contour(X, Y, Z, [z0 z0]);
%     curve = M(:, 2:end)';
%     dists = vecnorm(diff(curve), 2, 2);
%     s = cumsum([0; dists]);
%     curve_indices = round((curve - space(1)) / (8 / 300) + 1);
%     full_k = curvature(8 / 300, Z);
%     k = full_k(sub2ind(size(full_k), curve_indices(:, 2), curve_indices(:, 1)));
% end
% 

function plot_level_set(space, X, Y, Z, z0,i)
    fig=figure();
%     subplot(1, 2, 1);
    contour(X, Y, Z, [z0 z0]);
%     subplot(1, 2, 2);
%     [s, k] = curve_s_k(space, X, Y, Z, z0);
%     plot(s, k);
%     title(i);
%     saveas(fig,strcat('2_',string(i),'.png'))
end