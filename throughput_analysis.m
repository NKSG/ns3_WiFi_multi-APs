function throughput = throughput_analysis(n, datarate)
    W = 32;
    m = 5;
    my_function = strcat('x-1.0+(1.0-2.0*(1.0-2*x)/((1-2*x)*(', int2str(W), '+1)+x*'...
            , int2str(W), '*(1-(2*x)^', int2str(m), ')))^(', int2str(n), '-1)');
    p = fsolve(my_function, 0);
    tau = 2.0*(1.0-2*p)/((1-2*p)*(W+1)+p*W*(1-(2*p)^m));
    P_s = n*tau*(1-tau)^(n-1)/(1-(1-tau)^n);
    P_t = 1-(1-tau)^n;
    
    E_L = 1024*8; % bit
    T_sifs = 10; %us
    T_difs = 10+20*2; %us
    preamble = 192; %us
    Mac_header = 272; %bit
    ack_size = 112; % bit
% Notice that data using datarate to transmit, control using 1 Mbps to transmit
    T_S_BAS = preamble+(E_L+(Mac_header+preamble))/datarate+T_sifs+T_difs+(preamble+ack_size);
    T_C_BAS = preamble+(E_L+(Mac_header+preamble))/datarate+T_difs;
    T_S = T_S_BAS;
    T_C = T_C_BAS;
    sigma = 20;
    
    throughput = P_s*P_t*E_L/((1-P_t)*sigma+P_t*P_s*T_S+P_t*(1-P_s)*T_C);

end
