#include <tunings.h>

Tuning tuning_copy(Tuning of) {
    Tuning res;
    for (uint8_t i = 0; i < STRINGS_NUM; ++i) res.note_ids[i] = of.note_ids[i];
    for (uint8_t i = 0; i < TUNING_MAX_NAME_LENGTH; ++i) res.name[i] = of.name[i];
    return res;
}

Tuning TUNINGS[TUNINGS_NUM] = {
    { {28, 33, 38, 43, 47, 52}, "Std. E" },
    { {27, 32, 37, 42, 46, 51}, "Std. Eb" },
    { {26, 33, 38, 43, 47, 52}, "Drop D" },
    { {26, 33, 38, 42, 45, 50}, "Open D" },
    { {24, 33, 38, 43, 47, 52}, "Neon" },
    { {26, 33, 38, 43, 45, 50}, "DADGAD" }
};