throughput = [3.537 3.74823 3.72332 3.65321 3.63084 3.5827 3.54385 3.47332 3.49235 ...
              3.43491 3.25588 3.23145 3.22641 3.31553];
apNum = [1 2 3 4 5 6 7 8 9 10 15 20 25 30];

figure;
hold all;
set(gca, 'FontSize', 14, 'FontWeight', 'bold', 'FontName', 'Times New Roman'); 
xlabel('Number of APs', 'FontSize', 14, 'FontWeight', 'bold', 'FontName', 'Times New Roman');
ylabel('Throughput (Mbps)', 'FontSize', 14, 'FontWeight', 'bold', 'FontName', 'Times New Roman');
plot(apNum, throughput);
plot(apNum, throughput./apNum);
legend('Cumuulateive throughput',...
       'Average Throughput')
