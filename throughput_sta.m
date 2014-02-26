%=========================uplink+downlink=================================%
clear;close all;
throughput_ud = [5.34902 5.45974 5.34407 5.22924 5.13959 5.04606 4.95253 4.86205 ...
              4.78845 4.74641 4.47306 4.26366 4.11976 4.00793];
apNum = [1 2 3 4 5 6 7 8 9 10 15 20 25 30];
throughput_numerical_ud = [];
for i=1:size(apNum, 2)
    throughput_numerical_ud = [throughput_numerical_ud throughput_analysis(2*apNum(i), 11)];
end
figure;
hold all;
grid on;
set(gca, 'FontSize', 14, 'FontWeight', 'bold', 'FontName', 'Times New Roman'); 
title('Uplink and downlink, DSSS 11 Mbps', 'FontSize', 14, 'FontWeight', 'bold', 'FontName', 'Times New Roman')
xlabel('Number of APs', 'FontSize', 14, 'FontWeight', 'bold', 'FontName', 'Times New Roman');
ylabel('Throughput (Mbps)', 'FontSize', 14, 'FontWeight', 'bold', 'FontName', 'Times New Roman');
plot(apNum, throughput_ud, '-bo', 'LineWidth', 2, 'MarkerSize', 10);
plot(apNum, throughput_ud./apNum, '-rx', 'LineWidth', 2, 'MarkerSize', 15);    
plot(apNum, throughput_numerical_ud, '--bo', 'LineWidth', 2, 'MarkerSize', 10);
plot(apNum, throughput_numerical_ud./apNum, '--rx', 'LineWidth', 2, 'MarkerSize', 15);
legend('Sim. cumuulateive throughput',...
       'Sim. average Throughput',...
       'Ana. cumuulateive throughput',...
       'Ana. average Throughput')

%=========================downlink=================================%
throughput_d = [4.94521 5.35566 5.48879 5.47137 5.42626 5.33992 5.26742 5.23837...
           5.19271 5.1227 4.92321 4.74613 4.60444 4.46526];
apNum = [1 2 3 4 5 6 7 8 9 10 15 20 25 30];
throughput_numerical_d = [];
for i=1:size(apNum, 2)
    throughput_numerical_d = [throughput_numerical_d throughput_analysis(apNum(i), 11)];
end
figure;
hold all;
grid on;
set(gca, 'FontSize', 14, 'FontWeight', 'bold', 'FontName', 'Times New Roman'); 
title('Downlink, DSSS 11 Mbps', 'FontSize', 14, 'FontWeight', 'bold', 'FontName', 'Times New Roman')
xlabel('Number of APs', 'FontSize', 14, 'FontWeight', 'bold', 'FontName', 'Times New Roman');
ylabel('Throughput (Mbps)', 'FontSize', 14, 'FontWeight', 'bold', 'FontName', 'Times New Roman');
plot(apNum, throughput_d, '-bo', 'LineWidth', 2, 'MarkerSize', 10);
plot(apNum, throughput_d./apNum, '-rx', 'LineWidth', 2, 'MarkerSize', 15);

plot(apNum, throughput_numerical_d, '--bo', 'LineWidth', 2, 'MarkerSize', 10);
plot(apNum, throughput_numerical_d./apNum, '--rx', 'LineWidth', 2, 'MarkerSize', 15);
legend('Sim. cumuulateive throughput',...
       'Sim. average Throughput',...
       'Ana. cumuulateive throughput',...
       'Ana. average Throughput')


