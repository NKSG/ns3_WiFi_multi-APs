function throughput = throughput_analysis(n)
    % E_L = 8184;
    E_L = 8192;
    T_S_RTS = 9568;
    T_C_RTS = 417;
    T_S_BAS = 8982;
    T_C_BAS = 8713;
    T_S = T_S_BAS;
    T_C = T_C_BAS;
    sigma = 20;
    W = 32;
    m = 5;
    my_function = strcat('x-1.0+(1.0-2.0*(1.0-2*x)/((1-2*x)*(', int2str(W), '+1)+x*'...
            , int2str(W), '*(1-(2*x)^', int2str(m), ')))^(', int2str(n), '-1)');
    p = fsolve(my_function, 0);
    tau = 2.0*(1.0-2*p)/((1-2*p)*(W+1)+p*W*(1-(2*p)^m));
    P_s = n*tau*(1-tau)^(n-1)/(1-(1-tau)^n);
    P_t = 1-(1-tau)^n;
    throughput = 11*P_s*P_t*E_L/((1-P_t)*sigma+P_t*P_s*T_S+P_t*(1-P_s)*T_C);

end
