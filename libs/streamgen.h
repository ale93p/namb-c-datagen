double cbrdist(struct flow conf){
    return 1000/(double) conf.rate;
}

double burstdist(struct flow conf){
    time_t now;
    unsigned long timefromstart, timefromlastburst;

    time(&now);
    timefromstart = (unsigned long) now - (unsigned long) conf.starttime;
    timefromlastburst = timefromstart % (conf.bint + conf.bdur);

    if(
        timefromlastburst >= conf.bint &&
        timefromlastburst < conf.bint + conf.bdur
    ){
        return 0.;
    }
    else
    {
        return 1000/(double) conf.rate;
    }
}

/**
 * rate in msg/s 
 * phase in seconds of duration
 **/
double sindist(struct flow conf){
    double omega = 2 * PI * (1/(double)(conf.phase));
    time_t t;
    time(&t);
    double s = conf.rate/2 * sin(omega*(unsigned long)t) + conf.rate/2;
    if(s < 1) s = 1;
    return 1000/s; 
}

double sawdist(struct flow conf){
    double k = - (conf.rate / PI);
    if (conf.sdist == revsawtooth) k = k * -1.0;
    time_t t;
    time(&t);
    double omega = 1. / tan( ((unsigned long)t * PI) / conf.phase );
    double s = k * atan( omega ) + conf.rate/2;
    if ((int) s == 0) s = conf.rate;
    return 1000 / s;
}

double getint(struct flow conf){
    switch(conf.sdist){
        case cbr:
            return cbrdist(conf);
        break;
        case burst:
            return burstdist(conf);
        break;
        case sinusoidal:
            return sindist(conf);
        break;
        case sawtooth:
        case revsawtooth:
            return sawdist(conf);
        break;
    }
}