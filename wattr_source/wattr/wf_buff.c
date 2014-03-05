/*Defines a buffer for waveform samples from the ADE7753
 */

#include "wattr_mem.h"
#include "wattr_buffs.h"

static dqueue wf_buffer;
static uint32_t wf_mem[WF_BUFF_DPTH * WF_BUFF_WL];


