function path = shortest_path(T, source, target)
    rate = 1;
    [n, m] = size(T);
    X = 1:m;
    Y = 1:n;

    [Tx, Ty] = gradient(T);
    grad = cat(3, Tx, Ty);
    grad = grad ./ repmat(vecnorm(grad, 2, 3), [1 1 2]);
    Tx = grad(:, :, 1);
    Ty = grad(:, :, 2);
    
    path = target;
    while norm(path(end, :) - source) > eps
        last_point = path(end, :);
        x = last_point(1);
        y = last_point(2);
        
        interpolated_grad = [interp2(X, Y, Tx, x, y) interp2(X, Y, Ty, x, y)];
        path(end + 1, :) = last_point - rate * interpolated_grad;
    end
end

