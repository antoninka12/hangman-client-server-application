#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

/* typy komunikatów klient -> serwer */
#define TLV_LOGIN   1
#define TLV_JOIN    2
#define TLV_GUESS   3
#define TLV_WRONG   4
#define TLV_SCORE   5   

/* typy komunikatów serwer -> klient */
#define TLV_MSG     100

/* nagłówek TLV */
struct tlv_hdr {
    uint16_t type;
    uint16_t length;
};

/* limity pomocnicze */
#define MAX_TLV_VALUE 256
#define MAX_USERNAME  32

#endif