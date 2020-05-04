#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <libconfig.h>

#define FIRST_ASCII_VALUE 97
#define LAST_ASCII_VALUE 122
#define LETTERS 26
#define PI 3.1415926535
#define MS_TO_NS 1000000
#define NANOSEC 999999999

#define DEFAULT_CONF "generator.cfg"
#define OUT_PRE "./generator_"
#define OUT_SUF ".out"
#define OUT_NAME_SIZE 27

enum bool{true = 1, false = 0};

#include "cfgparser.h"
#include "datagen.h"
#include "streamgen.h"