unsigned char* next(long size, unsigned char* str){
    long j;
    char newval;
    long pivot = size - 1;
    for(j=0; j<size; ++j){
        newval = ((str[pivot] - FIRST_ASCII_VALUE) + 1) % LETTERS;
        str[pivot] = newval + FIRST_ASCII_VALUE;
        if(newval == 0) --pivot;
        else break;
    }
    return str;
}

void pldgen(unsigned char ** valuesarr, struct data conf){
    long i, j;
    long pivot;
    
    long size = conf.size;
    long values = conf.values;

    unsigned char newval;

    unsigned char *basestr = (unsigned char *) calloc(size, sizeof(unsigned char));
    for(i = 0; i<size; ++i) basestr[i] = FIRST_ASCII_VALUE;

    for(i = 0; i<values; ++i){
        valuesarr[i] = (unsigned char *) calloc(size, sizeof(unsigned char));
        for(j = 0; j<size; ++j) valuesarr[i][j] = basestr[j];
        basestr = next(size, basestr);
    }

    free(basestr);
}

/* Box-Muller Transform
*/
double genrnd(){
    /* Generate random namber between 0 and 1*/
    return ( (double)(rand()) + 1. )/( (double)(RAND_MAX) + 1. );
}

double normalrnd(){
    double u1 = genrnd();
    double u2 = genrnd();
    double z = sqrt(-2*log(u1)) * cos(2*PI*u2);
    return z;
}

long indexgen(struct data conf){
    double val;
    switch(conf.ddist){
        case uniform:
            return rand() % conf.values;
        break;
        case nonuniform:
        /* limit the generated numbers */
            do{
            val = normalrnd();   
            }while(val < -2.5 || val > 2.5);
            /* return a value between 0 and the given array size */
            return (long)((val + 2.5) / 5. * (double) conf.size);
        break;
    }
    
}