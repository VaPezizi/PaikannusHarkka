#include "endians.h" 

uint16_t swapEndian16(uint16_t val) {
    uint16_t left = (val & 0xff);
    uint16_t right = (val >> 8) & 0xff;
    return ((left) | (right));
}
uint32_t swapEndian32(uint32_t val) {
    uint32_t left = 0, mleft = 0, mright = 0, right = 0;
    left = (val & 0xff);
    mleft = (val >> 8) & 0xff;
    mright = (val >> 16) & 0xff;
    right = (val >> 24) & 0xff;
    return (left |
           mleft |
           mright |
            right);
}
