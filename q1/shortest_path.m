function path = shortest_path(T, source, target)
    % Get matrix size
    [h, w] = size(T);
    
    % Generate horizontal indices
    X = 1:w;
    
    % Generate vertical indices
    Y = 1:h;

    % Calculate gradient at every grid point using finite differences
    [Tx, Ty] = gradient(T);
    
    % Concatenate Tx (x coordinates of gradients) and Ty (y coordinates of
    % gradients) along the third dimention
    g = cat(3, Tx, Ty);
    
    % Calculate gradient norms
    g_norm = vecnorm(g, 2, 3);
    g_norm_rep = cat(3, g_norm, g_norm);
    
    % Normalize gradients
    g = g ./ g_norm_rep;
    
    Tx = g(:, :, 1);
    Ty = g(:, :, 2);
    
    % -----------------------------------------------------------------
    % We will walk from the target to the source using gradient descent
    % -----------------------------------------------------------------
    
    % Set 'target' as the starting point
    path = target;
    
    % Continue till we reach 'source'
    while norm(path(end, :) - source) > eps
        prev_point = path(end, :);
        
        % Get the x and y coordinates of the previous point in path
        x = prev_point(1);
        y = prev_point(2);
        
        % Interpolate the gradient between grid points
        interpolated_g = [interp2(X, Y, Tx, x, y) interp2(X, Y, Ty, x, y)];
        
        % Append the path with the next point using gradient descent
        path(end + 1, :) = prev_point - interpolated_g;
    end
end

