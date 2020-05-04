#include "libs/namb.h"
#include <librdkafka/rdkafka.h>

enum bool run = true;

static void formatted_help_line(char *cmd, char *helper);
static void help(char *cmd_name);
static void stop (int sig);

int main(int argc, char *argv[]){
    char *cfgfile = NULL;
   

    for(int i=1; i<argc; ++i){
        if(strcmp(argv[i],"-h")==0 || strcmp(argv[i],"--help")==0) 
            help(argv[0]);

        if(strcmp(argv[i],"-c")==0 || strcmp(argv[i],"--conf")==0){
            cfgfile = argv[++i];
        }
    }

    /* configurations */
    struct nambcfg conf;
    struct kafkacfg kafka;
    struct globalcfg glob;

    if(cfgfile == NULL){
        nambcfg(&conf, DEFAULT_CONF);
        kafkacfg(&kafka, DEFAULT_CONF);
        globalcfg(&glob, DEFAULT_CONF);
    }
    else{
        nambcfg(&conf, cfgfile);
        kafkacfg(&kafka, cfgfile);
        globalcfg(&glob, cfgfile);
    }
    
    // /* kafka */
    rd_kafka_t *rk;         /* Producer instance handle */
    rd_kafka_conf_t *kafka_conf;  /* Temporary configuration object */
    char errstr[512];       /* librdkafka API error reporting buffer */
    rd_kafka_resp_err_t err;

    // /* local variables */
    srand(time(NULL)); /* initialize time for index generation */
    unsigned long j, idx;

    double interval;
    unsigned long interval_nsec;
    unsigned long sec, nsec, overtime_sec, overtime_usec;
    
    struct timeval ot1, ot2;
    struct timespec sleep_time, sleep_ret;
    
    unsigned long counter = 0;
    char outname[OUT_NAME_SIZE+1];
    FILE *outfile = NULL;

    
    /* Start Kafka Client */
    kafka_conf = rd_kafka_conf_new();
    if (rd_kafka_conf_set(kafka_conf, "bootstrap.servers", kafka.broker,
                            errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK) {
            fprintf(stderr, "%s\n", errstr);
            return 1;
    }
    rk = rd_kafka_new(RD_KAFKA_PRODUCER, kafka_conf, errstr, sizeof(errstr));
    if (!rk) {
        fprintf(stderr,
                "%% Failed to create new producer: %s\n", errstr);
        return 1;
    }

    /* setup output file */
    if(glob.debug > 0){
        snprintf(outname, OUT_NAME_SIZE, "%s%ld%s", OUT_PRE, (unsigned long) time(NULL), OUT_SUF);
        outfile = fopen(outname, "w");
    }

    /* Signal handler for clean shutdown */
    signal(SIGINT, stop);

    /* generate dataset */
    unsigned char **dataset = (unsigned char **) calloc(conf.data.values, sizeof(unsigned char*));
    pldgen(dataset, conf.data);

    /* main loop */
    unsigned char *sndval = (unsigned char *) calloc(conf.data.size, sizeof(unsigned char));

    time(&conf.flow.starttime);
    while(run){
        gettimeofday(&ot1, NULL);

        /* get next value to send */
        idx = indexgen(conf.data);
        for(j = 0; j<conf.data.size; ++j) sndval[j] = dataset[idx][j];

        /* Send Value to Kafka */
        retry:
            err = rd_kafka_producev(
                rk,
                RD_KAFKA_V_TOPIC(kafka.topic),
                RD_KAFKA_V_MSGFLAGS(RD_KAFKA_MSG_F_COPY),
                RD_KAFKA_V_VALUE(sndval, conf.data.size),
                RD_KAFKA_V_OPAQUE(NULL),
                /* End sentinel */
                RD_KAFKA_V_END);
            
            if (err) {
                if (err == RD_KAFKA_RESP_ERR__QUEUE_FULL) {
                    rd_kafka_poll(rk, 1000/*block for max 1000ms*/);
                    goto retry;
                }
            }
            rd_kafka_poll(rk, 0/*non-blocking*/);

        
        if(glob.debug > 0 && (counter++ % (int)(1/glob.debug)) == 0)
            fprintf(outfile, "%ld,%ld,%s\n", counter, (unsigned long) time(NULL), sndval);

        interval = getint(conf.flow); 

        interval_nsec = (unsigned long)(interval * MS_TO_NS);
        sec = (unsigned long) (interval_nsec / MS_TO_NS / 1000);
        nsec = (unsigned long) (interval_nsec - (sec * MS_TO_NS * 1000));

        /* remove offset from sleeptime */
        gettimeofday(&ot2, NULL);
        overtime_sec = (unsigned long) (ot2.tv_sec - ot1.tv_sec);
        overtime_usec = (unsigned long) (ot2.tv_usec - ot1.tv_usec);
        
        if(overtime_sec >= sec) sec = 0;
        else if(sec > 0) sec -= overtime_sec;
        if( nsec - (overtime_usec * 1000) < 0 ){
            if (sec > 0) --sec; 
            nsec = NANOSEC - ( (nsec - (overtime_usec * 1000)) * -1 ) ; 
        }
        
        sleep_time.tv_sec = sec;
        sleep_time.tv_nsec = nsec;

        nanosleep(&sleep_time, &sleep_ret);
    }    

    printf("Closing\n");
    rd_kafka_flush(rk, 10*1000 /* wait for max 10 seconds */);
    rd_kafka_destroy(rk);
    free(sndval);
    free(dataset);
    if(glob.debug > 0) fclose(outfile);
    return 0;
}

static void formatted_help_line(char *cmd, char *helper){
    printf("\t%-20s %-40s\n", cmd, helper);
}

static void help(char *cmd_name){
    printf("usage: %s [opts]\n"
            "Options are:\n", cmd_name);
    formatted_help_line("-h,--help", "display what you are reading now");
    formatted_help_line("-c,--conf <path>", "specify config file");
    
    exit(EXIT_FAILURE);
}

static void stop (int sig) {
    run = false;
}