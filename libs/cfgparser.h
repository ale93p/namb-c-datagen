struct globalcfg {
    double debug;
};

struct nambcfg {
    struct data
    {
        unsigned long values;
        unsigned long size;
        enum datadist{uniform, nonuniform} ddist;       
    } data;
    struct flow
    {
        unsigned long rate;
        unsigned long phase;
        unsigned long bint;
        unsigned long bdur;
        time_t starttime;
        enum streamdist{cbr, burst, sawtooth, revsawtooth, sinusoidal} sdist;
    } flow;
};

#define ENTRY(x) { x, #x }

typedef struct { 
    enum datadist dist;
    const char* const str;
} ddistvalues;

enum datadist str_to_ddist(const char* const s) {
    static ddistvalues map[] = {
        ENTRY(uniform),
        ENTRY(nonuniform)
    }; 
    static const unsigned size = sizeof(map) / sizeof(map[0]);

    for(unsigned i=0; i<size; ++i) {
         if(strcmp(map[i].str, s) == 0) 
             return map[i].dist;
    }

    return -1;
}

typedef struct { 
    enum streamdist dist;
    const char* const str;
} sdistvalues;

enum streamdist str_to_sdist(const char* const s) {
    static sdistvalues map[] = {
        ENTRY(cbr),
        ENTRY(burst),
        ENTRY(sinusoidal),
        ENTRY(sawtooth),
        ENTRY(revsawtooth)
    }; 
    static const unsigned size = sizeof(map) / sizeof(map[0]);

    for(unsigned i=0; i<size; ++i) {
         if(strcmp(map[i].str, s) == 0) 
             return map[i].dist;
    }

    return -1;
}
#undef SDIST_VALUE_OF

int nambcfg(struct nambcfg *conf, const char *filename){
    config_t cfg;
    config_setting_t *setting;
    const char *strval;
    long long i64val;
    
    config_init(&cfg);

    if(! config_read_file(&cfg, filename))
    {
        fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg),
                config_error_line(&cfg), config_error_text(&cfg));
        config_destroy(&cfg);
        return(EXIT_FAILURE);
    }

    setting = config_lookup(&cfg, "datastream.data");
    if(setting != NULL)
    {   
        if(config_setting_lookup_int64(setting, "values", &i64val))
            conf->data.values = i64val;

        if(config_setting_lookup_int64(setting, "size", &i64val))
            conf->data.size = i64val;
        
        if(config_setting_lookup_string(setting, "ddist", &strval))
            conf->data.ddist = str_to_ddist(strval); 
    }

    setting = config_lookup(&cfg, "datastream.flow");
    if(setting != NULL)
    {   
        if(config_setting_lookup_int64(setting, "rate", &i64val))
            conf->flow.rate = i64val;

        if(config_setting_lookup_int64(setting, "phase", &i64val))
            conf->flow.phase = i64val;
        
        if(config_setting_lookup_int64(setting, "bint", &i64val))
            conf->flow.bint = i64val;
        
        if(config_setting_lookup_int64(setting, "bdur", &i64val))
            conf->flow.bdur = i64val;
        
        if(config_setting_lookup_string(setting, "sdist", &strval))
            conf->flow.sdist = str_to_sdist(strval);  
    }

    return 0;
}

struct kafkacfg {
    const char *broker;
    const char *topic;
};

int kafkacfg(struct kafkacfg *conf, const char *filename){
    config_t cfg;
    config_setting_t *setting;
    const char *strval;
    
    config_init(&cfg);

    if(! config_read_file(&cfg, filename))
    {
        fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg),
                config_error_line(&cfg), config_error_text(&cfg));
        config_destroy(&cfg);
        return(EXIT_FAILURE);
    }

    setting = config_lookup(&cfg, "kafka");
    if(setting != NULL)
    {     
        if(config_setting_lookup_string(setting, "broker", &strval))
            conf->broker = strval;  
        if(config_setting_lookup_string(setting, "topic", &strval))
            conf->topic = strval; 
    }

    return 0;
}

int globalcfg(struct globalcfg *conf, const char *filename){
    config_t cfg;
    config_setting_t *setting;
    double doubleval;

    /* defaults */
    conf->debug = 0;
    
    config_init(&cfg);

    if(! config_read_file(&cfg, filename))
    {
        fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg),
                config_error_line(&cfg), config_error_text(&cfg));
        config_destroy(&cfg);
        return(EXIT_FAILURE);
    }

    setting = config_lookup(&cfg, "global");
    if(setting != NULL)
    {     
        if(config_setting_lookup_float(setting, "debug", &doubleval))
            conf->debug = doubleval;
    }

    return 0;
}