# C Data Generator for NAMB

Data Generator for NAMB, implemented in C.

## Features
* Overcomes NAMB interal generator limitations (e.g. Java sleep time)
* Can be adapted to every system 
* Kafka Producer already implemented

## Requirements
* Compiler for C99
* librdkafka: [edenhill/librdkafka](https://github.com/edenhill/librdkafka)
* libconfig: [hyperrealm/libconfig](https://hyperrealm.github.io/libconfig/)

## Quick Start
### Compile the source
```
gcc -lrdkafka -lconfig -o generator generator.c
```

### Configure the generator
Customize the file `namb.cfg` following the same [parameters for the synthetic generator](https://apgl.xyz/namb/config/workflow.html#synthetic-datastream) in NAMB.

### Run the generator
```
./generator
```

### Implement your own generator
If you want to optimize it, adapt for some other middleware rather than Kafka, just follow the example of the ```generator.c``` file and adapt it to your needs.