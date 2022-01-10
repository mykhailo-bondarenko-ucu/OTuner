#ifndef _TUNINGS_H
#define _TUNINGS_H

#include <stdint.h>

#define TUNINGS_NUM 1
#define STRINGS_NUM 6
#define TUNING_MAX_NAME_LENGTH 10

//  Structure used to define tunings
typedef struct {
    uint8_t note_ids[STRINGS_NUM];
    char name[TUNING_MAX_NAME_LENGTH];
} Tuning;

//  Export tunings
extern Tuning TUNINGS[];

//  Helper functions
Tuning tuning_copy(Tuning of);

#endif