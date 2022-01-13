#!/bin/bash

st-info --probe --connect-under-reset
st-flash write ./.pio/build/genericSTM32F103C8/firmware.bin 0x8000000
