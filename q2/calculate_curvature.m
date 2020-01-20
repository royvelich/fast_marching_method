function k = calculate_curvature(Z)
    % Approximate first-order and second-order derivatives
    [Zx, Zy] = gradient(Z);
    [Zxx, Zxy] = gradient(Zx);
    [~, Zyy] = gradient(Zy);
    
    % Calculate curvature at each point on the grid
    % https://en.wikipedia.org/wiki/Implicit_curve
    Zx2 = Zx .^ 2;
    Zy2 = Zy .^ 2;
    k = -(Zxx .* Zy2 - 2 * Zx .* Zy .* Zxy + Zyy .* Zx2) ./ ((Zx2 + Zy2) .^ (3/2));
end

