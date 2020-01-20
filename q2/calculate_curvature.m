function k = calculate_curvature(Z)
    [Zx, Zy] = gradient(Z, h);
    [Zxx, Zxy] = gradient(Zx, h);
    [~, Zyy] = gradient(Zy, h);
    Zx2 = Zx .^ 2;
    Zy2 = Zy .^ 2;
    k = - (Zxx .* Zy2 - 2 * Zx .* Zy .* Zxy + Zyy .* Zx2) ./ ((Zx2 + Zy2) .^ (3/2));
end

