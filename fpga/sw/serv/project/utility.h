#ifndef UTILITY
#define UTILITY

    #define DATA_0(r, c) (DATA_0[(r)*FMCS + (c)]) // Map 1D array to 2D array
    #define DATA_1(r, c) (DATA_1[(r)*SMCS + (c)])
    #define DATA_2(r, c) (DATA_2[(r)*SMCS + (c)])
    #define DATA_3(r, c) (DATA_3[(r)*SMCS + (c)])
    
    void failed(void);
    void passed(void);
    void tic(void);
    unsigned int toc(void);
#endif // UTILITY
