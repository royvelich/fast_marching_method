function plot_path(path, color)
    plotted_path = plot(path(:, 1), path(:, 2), color);
    set(plotted_path,'LineWidth',2);
end

