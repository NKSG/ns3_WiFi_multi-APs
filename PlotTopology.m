function PlotTopology(devicePosX, devicePosY, aptCorner)
figure
hold on
for i = 1:size(devicePosX, 1)/2
  plot(devicePosX(i, 1), devicePosY(i, 1), 'k^', 'LineWidth', 2);
end
for i = size(devicePosX, 1)/2+1:size(devicePosX, 1)
  plot(devicePosX(i, 1), devicePosY(i, 1), 'bo', 'LineWidth', 2);
end
aptSizeX = 50.0;
aptSizeY = 20.0;
roadSize = 10.0;
for i = 1:size(aptCorner, 1)
  for j = 1:size(aptCorner, 2)
    if aptCorner(i, j) == 1.0
      aptColor = rand(3, 1);
      rectangle('Position', [(i-1)*(aptSizeX+roadSize) (j-1)*(aptSizeY+roadSize) aptSizeX aptSizeY], 'LineWidth', 2, 'edgecolor',...
        [aptColor(1, 1) aptColor(2, 1) aptColor(3, 1)]);
      
      plot([(i-1)*(aptSizeX+roadSize)+aptSizeX (i-1)*(aptSizeX+roadSize)], [(j-1)*(aptSizeY+roadSize)+aptSizeY/2 (j-1)*(aptSizeY+roadSize)+aptSizeY/2], 'LineWidth', 2, 'Color',...
        [aptColor(1, 1) aptColor(2, 1) aptColor(3, 1)]);
      for k=1:5
        plot([(i-1)*(aptSizeX+roadSize)+aptSizeX/5*k (i-1)*(aptSizeX+roadSize)+aptSizeX/5*k], [(j-1)*(aptSizeY+roadSize)+aptSizeY (j-1)*(aptSizeY+roadSize)], 'LineWidth', 2, 'Color',...
          [aptColor(1, 1) aptColor(2, 1) aptColor(3, 1)]);
      end
    end
  end
end
end
