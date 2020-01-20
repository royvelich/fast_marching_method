function curve_k = plot_evolution(grid, X, Y, Z, ratio, grid_length)
    % plot current curve evolution
    figure();
    subplot(1, 2, 1);
    C = contour(X, Y, Z, [0 0]);
    
    % remove first column
    C = C(:, 2:end);
    
    % calculate curvature over the whole grid
    k = calculate_curvature(Z);
    
    % Find the grid cell indices corresponding to the curve
    indices = round((C + grid_length) * ratio + 1);
    
    % sample the curvature at the cells corresponding to the curve points
    curve_k = k(sub2ind(size(k), indices(2, :), indices(1, :)));
    
    % get arc length at each curve point
    s = calculate_arc_length(C);
    
    % plot k as a function of s
    subplot(1, 2, 2);
    plot(s, curve_k);
end

