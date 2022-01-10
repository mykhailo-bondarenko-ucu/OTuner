#ifndef _NOTES_H
#define _NOTES_H

#include <stdint.h>

// Structure constants
#define NOTE_FONT_FACE_WIDTH 32
#define NOTE_FONT_FACE_HEIGHT 26
#define NOTES_NUM 108

// Structure used to define individual notes
typedef struct {
    float freq;
    char name[4];
    uint32_t fontFace[NOTE_FONT_FACE_HEIGHT];
} Note;

// Export notes
extern Note NOTES[];

#endif