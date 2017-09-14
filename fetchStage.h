#ifndef FETCHSTAGE_H
#define FETCHSTAGE_H
#include "forwarding.h"
#include "bubbling.h"
typedef struct 
{
    unsigned int predPC;
} fregister;

//prototypes for functions called from files other than fetchStage
fregister getFregister();
void fetchStage(forwardType forward, bubbleType bubble);
void clearFregister();
#endif
