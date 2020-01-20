% Number of grid samples
n = 500;

% Grid length
grid_length = 5;

% grid samples to grid length ratio
ratio = n / (2 * grid_length);

% Sample 2D grid
grid = linspace(-grid_length, grid_length, n);

% Get coordinates of grid points
[X, Y] = meshgrid(grid);

% Generate two paraboloids shifted from each other along the X axis. Add a
% vertical negative offset so we can slice the surface at z = 0
paraboloid1 = (X - 2) .^ 2 + Y .^ 2 - 5;
paraboloid2 = (X + 2) .^ 2 + Y .^ 2 - 5;

% Creeate the desired surface (with two bumps) by taking the min over Z
Z = min(paraboloid1, paraboloid2);

% small time steps
dt = 1e-1;

% plot evolution every few iterations
plot_every = 100;

for i = 0 : 500
    % Plot curve evolution every few iterations
    if mod(i, plot_every) == 0
        plot_evolution(i, X, Y, Z, ratio, grid_length);
    end
    
    % Approximate surface normal (implicit function gradient) using finite
    % differences
    [Zx, Zy] = gradient(Z);
    
    % Concatenate partial derivatives
    Zg = cat(3, Zx, Zy);
    
    % Calculate gradient norm at each grid point
    g_norm = vecnorm(Zg, 2, 3);
    
    % Evolve Z by infinitesimal delta of gradient norm
    Z = Z + dt * g_norm;
end