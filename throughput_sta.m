%=========================downlink=================================%
throughput_d = [3.537 3.74823 3.72332 3.65321 3.63084 3.5827 3.54385 3.47332 3.49235 ...
              3.43491 3.25588 3.23145 3.22641 3.31553];
apNum = [1 2 3 4 5 6 7 8 9 10 15 20 25 30];

figure;
hold all;
grid on;
title('Downlink only, DSSS 11 Mbps', 'FontSize', 14, 'FontWeight', 'bold', 'FontName', 'Times New Roman')
set(gca, 'FontSize', 14, 'FontWeight', 'bold', 'FontName', 'Times New Roman'); 
xlabel('Number of APs', 'FontSize', 14, 'FontWeight', 'bold', 'FontName', 'Times New Roman');
ylabel('Throughput (Mbps)', 'FontSize', 14, 'FontWeight', 'bold', 'FontName', 'Times New Roman');
plot(apNum, throughput_d, '-bo', 'LineWidth', 2, 'MarkerSize', 10);
plot(apNum, throughput_d./apNum, '-rx', 'LineWidth', 2, 'MarkerSize', 15);
legend('Cumuulateive throughput',...
       'Average Throughput')
   
%=========================uplink=================================%
throughput_u = [3.69541 3.75288 3.70136 3.65702 3.59536 3.57129 3.52864 ...
              3.47459 3.49446 3.38675 3.30066 3.20822 3.28258 3.32655];
apNum = [1 2 3 4 5 6 7 8 9 10 15 20 25 30];

figure;
hold all;
grid on;
title('Uplink only, DSSS 11 Mbps', 'FontSize', 14, 'FontWeight', 'bold', 'FontName', 'Times New Roman')
set(gca, 'FontSize', 14, 'FontWeight', 'bold', 'FontName', 'Times New Roman'); 
xlabel('Number of APs', 'FontSize', 14, 'FontWeight', 'bold', 'FontName', 'Times New Roman');
ylabel('Throughput (Mbps)', 'FontSize', 14, 'FontWeight', 'bold', 'FontName', 'Times New Roman');
plot(apNum, throughput_u, '-bo', 'LineWidth', 2, 'MarkerSize', 10);
plot(apNum, throughput_u./apNum, '-rx', 'LineWidth', 2, 'MarkerSize', 15);
legend('Cumuulateive throughput',...
       'Average Throughput')

%=========================uplink+downlink=================================%
throughput_ud = [5.66478 5.62173 5.51277 5.39368 5.24586 5.1783 5.10694 5.03051 ...
              4.94183 4.87977 4.60872 4.42892 4.25618 4.148];
apNum = [1 2 3 4 5 6 7 8 9 10 15 20 25 30];
figure;
hold all;
grid on;
set(gca, 'FontSize', 14, 'FontWeight', 'bold', 'FontName', 'Times New Roman'); 
title('Uplink and downlink, DSSS 11 Mbps', 'FontSize', 14, 'FontWeight', 'bold', 'FontName', 'Times New Roman')
xlabel('Number of APs', 'FontSize', 14, 'FontWeight', 'bold', 'FontName', 'Times New Roman');
ylabel('Throughput (Mbps)', 'FontSize', 14, 'FontWeight', 'bold', 'FontName', 'Times New Roman');
plot(apNum, throughput_ud, '-bo', 'LineWidth', 2, 'MarkerSize', 10);
plot(apNum, throughput_ud./apNum, '-rx', 'LineWidth', 2, 'MarkerSize', 15);
legend('Cumuulateive throughput',...
       'Average Throughput')
