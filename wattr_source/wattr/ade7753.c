#include "headers/wattr_mem.h"
#include "headers/ade7753.h"
#include "headers/wattr_queue"

//Write buffer for ADE7753 register access commands
static uint32_t ade_txbuff[ADE_RXBUFF_WL * 3];
//buffer of received data from ADE7753
static uint32_t ade_rxbuff[ADE_RXBUFF_WL * ADE_RXBUFF_DPTH]

static 
