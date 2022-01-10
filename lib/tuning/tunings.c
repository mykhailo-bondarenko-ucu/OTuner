#include <tunings.h>

Tuning tuning_copy(Tuning of) {
    Tuning res;
    for (uint8_t i = 0; i < STRINGS_NUM; ++i) res.note_ids[i] = of.note_ids[i];
    for (uint8_t i = 0; i < TUNING_MAX_NAME_LENGTH; ++i) res.name[i] = of.name[i];
    return res;
}

Tuning TUNINGS[TUNINGS_NUM] = {
    { {28, 33, 38, 43, 47, 52}, "Std. E4" }
};