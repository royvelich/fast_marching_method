function s = calculate_arc_length(C)
    % calculate distance between adjacent points
    D = diff(C,1,2);
    
    % Calculate the diff vectors 2-norm
    D_norm = vecnorm(D, 2, 1);
    
    % Calculate the accumulated distance at each point
    s = cumsum([0, D_norm]);
end

