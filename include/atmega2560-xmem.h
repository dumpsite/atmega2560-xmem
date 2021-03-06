/**
 * Extended Memory interface for the Atmega2560 MCU.
 *
 * Configuration file.
 *
 * @author Francisco Soto <francisco@nanosatisfi.com>
 ******************************************************************************/

#ifndef ATMEGA2560_XMEM_H_INCLUDED
#define ATMEGA2560_XMEM_H_INCLUDED

#ifndef CONF_XMEM_H_INCLUDED
#error "Include conf_xmem.h before atmega2560-xmem.h."
#endif

#if XMEM_WAIT_STATES < 0 || XMEN_WAIT_STATES > 3
#error "XMEM_WAIT_STATES should be a number between 0 and 3."
#endif

#include <stdint.h>

void xmem_switch_bank (uint8_t bank);
void xmem_init (void);
void *xmem_unshadow_lower_memory (void);
void xmem_shadow_lower_memory (void);
void xmem_set_xmem_heap (void);
void xmem_set_system_heap (void);
void *xmem_get_current_bank_address_start (void);
void *xmem_get_current_bank_address_end (void);

/* How many memory banks are there? */
#if XMEM_TOTAL_MEMORY < 65536
#define XMEM_BANKS           1
#else
#define XMEM_BANKS           ((uint8_t)((XMEM_TOTAL_MEMORY / 65536.0)+0.5))
#define XMEM_USE_BANKING
#endif

#endif /* CONF_XMEM_H_INCLUDED */
