#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>
 
//typ 
#define TLV_LOGIN   1
#define TLV_JOIN    2
#define TLV_GUESS   3
#define TLV_WRONG   4
#define TLV_SCORE   5   

#define TLV_MSG     100

//nagłówek
struct tlv_hdr {
    uint16_t type;
    uint16_t length;
};

//limity
#define MAX_TLV_VALUE 256
#define MAX_USERNAME  32

#endif
