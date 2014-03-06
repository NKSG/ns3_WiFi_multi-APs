function [devicePosX devicePosY distanceDiff channelGainDb aptCorner] ...
  = GenerateChannelGain(buildingNumX, buildingNumY)
% Paramter is reference from 3GPP TR36814
% Apartment position
aptSizeX = 50.0;
aptSizeY = 20.0;
roadSize = 10.0;
aptCorner = ones(buildingNumX, buildingNumY);
devicePosX = []; devicePosY = [];
% Generate APs
for i=1:buildingNumX
  for j=1:buildingNumY
    devicePosX = [devicePosX; (5:10:45)'+(i-1)*60; (5:10:45)'+(i-1)*60];
    devicePosY = [devicePosY; ones(5, 1)*5+(j-1)*30; ones(5, 1)*15+(j-1)*30];
  end
end
% Generate STAs
for i=1:buildingNumX
  for j=1:buildingNumY
    devicePosX = [devicePosX; (5:10:45)'+(i-1)*60+2.5; (5:10:45)'+(i-1)*60+2.5];
    devicePosY = [devicePosY; ones(5, 1)*5+(j-1)*30; ones(5, 1)*15+(j-1)*30];
  end
end
distanceDist = pdist([devicePosX  devicePosY]);
distanceDiff = squareform(distanceDist);

inBuilding = zeros(size(devicePosX, 1), 2);
for i = 1:size(devicePosX, 1)
  xCorner = floor(devicePosX(i, 1)/(aptSizeX+roadSize))*(aptSizeX+roadSize);
  yCorner = floor(devicePosY(i, 1)/(aptSizeY+roadSize))*(aptSizeY+roadSize);
  if (devicePosX(i, 1)-xCorner) <= aptSizeX && (devicePosY(i, 1)-yCorner) <= aptSizeY
    inBuilding(i, 1) = xCorner/(aptSizeX+roadSize)+1;
    inBuilding(i, 2) = yCorner/(aptSizeY+roadSize)+1;
  end
end

channelGainDb = max(2.7+42.8*log10(distanceDiff), 38.46+20.0*log10(distanceDiff))+18.3+20;

% ==In the same building
for i = 1:size(devicePosX, 1)
  same_building = find(inBuilding(:, 1)==inBuilding(i, 1) & inBuilding(:, 2)==inBuilding(i, 2));
  x_corner = floor(devicePosX(i, 1)/(aptSizeX+roadSize))*(aptSizeX+roadSize);
  y_corner = floor(devicePosY(i, 1)/(aptSizeY+roadSize))*(aptSizeY+roadSize);
  for j = 1:size(same_building)
    apt_num_x_i = floor((devicePosX(i, 1)-x_corner)/(aptSizeX/2));
    apt_num_y_i = floor((devicePosY(i, 1)-y_corner)/(aptSizeY/2));
    apt_num_x_j = floor((devicePosX(j, 1)-x_corner)/(aptSizeX/2));
    apt_num_y_j = floor((devicePosY(j, 1)-y_corner)/(aptSizeY/2));
    if apt_num_x_i==apt_num_x_j && apt_num_y_i==apt_num_y_j
      channelGainDb(i, same_building) = 38.46+20.0*log10(distanceDiff(i, same_building))+18.3;
      channelGainDb(same_building, i) = 38.46+20.0*log10(distanceDiff(i, same_building))+18.3;
    else
      channelGainDb(i, same_building) = 38.46+20.0*log10(distanceDiff(i, same_building))+18.3+5;
      channelGainDb(same_building, i) = 38.46+20.0*log10(distanceDiff(i, same_building))+18.3+5;
    end
  end
end
% ==In the different building
for i = 1:size(devicePosX, 1)
  different_building = find(inBuilding(:, 1)~=0 & inBuilding(:, 1)~=inBuilding(i, 1)...
    & inBuilding(:, 2)~=inBuilding(i, 2));
  channelGainDb(i, different_building) = max(2.7+42.8*log10(distanceDiff(i, different_building)),...
            38.46+20*log10(distanceDiff(i, different_building)))+18.3+5+20+20;
  channelGainDb(different_building, i) = max(2.7+42.8*log10(distanceDiff(i, different_building)),...
           38.46+20*log10(distanceDiff(i, different_building)))+18.3+5+20+20;
end

end
