#include "endians.h" 

uint16_t swapEndian16(uint16_t val) {
    uint8_t left = (val & 0xff);
    uint8_t right = (val >> 8) & 0xff;
    return ((left) | (right));
}
uint32_t swapEndian32(uint32_t val) {
    uint8_t left, mleft, mright, right;
    left = (val & 0xff);
    mleft = (val >> 8) & 0xff;
    mright = (val >> 16) & 0xff;
    right = (val >> 24) & 0xff;
    return (left |
           mleft |
           mright |
            right);
}
