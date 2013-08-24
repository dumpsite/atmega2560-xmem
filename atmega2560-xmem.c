/**
 * Extended Memory interface for the Atmega2560 MCU.
 *
 * @author Francisco Soto <francisco@nanosatisfi.com>
 ******************************************************************************/

#include <stdlib.h>
#include <avr/io.h>

#include "conf_xmem.h"
#include "atmega2560-xmem.h"

/* Atmega XMEM address space block */
#define XMEM_START      ((void *)0x2200)
#define XMEM_END        ((void *)0xffff)

struct bank_heap_state {
    void *__brkval;             /* Pointer between __malloc_heap_start and __malloc_heap_end, shows growth. */
    void *__flp;                /* Pointer to the free block list that malloc handles. */
    char *__malloc_heap_start;  /* Pointer to the beginning of the heap. */
    char *__malloc_heap_end;    /* Pointer to the end of the heap, 0 if the heap is below the stack. */
};

/* Private heap variables */
#ifdef __cplusplus
extern "C" {
#endif
    extern void *__flp;
    extern void *__brkval;
#ifdef __cplusplus
}
#endif

struct bank_heap_state _bank_state[XMEM_BANKS];
uint8_t _current_bank = 0;

/**
 * @docstring
 * Save the current heap state in the given bank_heap_state pointer.
 */
static void _xmem_save_bank_state (struct bank_heap_state *bs) {
    bs->__brkval = __brkval;
    bs->__flp = __flp;
    bs->__malloc_heap_start = __malloc_heap_start;
    bs->__malloc_heap_end = __malloc_heap_end;
}

/**
 * @docstring
 * Load the given bank_heap_state into the global heap state.
 */
static void _xmem_load_bank_state (struct bank_heap_state *bs) {
    __brkval = bs->__brkval;
    __flp = bs->__flp;
    __malloc_heap_start = bs->__malloc_heap_start;
    __malloc_heap_end = bs->__malloc_heap_end;
}

/**
 * @docstring
 * Switch bank if the bank exist and is not the current one.
 */
void xmem_switch_bank (uint8_t bank) {
    if (_current_bank == bank || bank > XMEM_BANKS) {
        return;
    }

#ifdef XMEM_HEAP_IN_XMEM
/* Save the current bank heap state */
    _xmem_save_bank_state(&_bank_state[_current_bank]);

/* Load the bank heap state */
    _xmem_load_bank_state(&_bank_state[bank]);
#endif /* XMEM_HEAP_IN_XMEM */

    _current_bank = bank;

/* Have the user set the higher bits */
    XMEM_USER_SWITCH_BANK(bank);
}

/**
 * @docstring
 * Initializes the external memory and the internal data structures if we are managing the heap in the xmem.
 */
void xmem_init () {
/* TODO: Calculate the # of pins we need for the given memory and
   calculate a mask for XMCRB. Save this mask because we will need it
   to restore XMCRB later on if we want to access the first 8KB of XMEM.
   This assumes we want all 8 bits from PORTC. */
    XMCRB = 0;

/* XMEM Enable bit, entire xmem is treated like one sector and set the wait
   states for it. */
    XMCRA = (1 << SRE) | (XMEM_WAIT_STATES << SRW10);

/* Have the user configure his extra pins */
    XMEM_USER_INIT();

#ifdef XMEM_HEAP_IN_XMEM
/* If heap should be in xmem ram then we should let avr-libc where is it. */
    __malloc_heap_start = (char *)XMEM_START;
    __malloc_heap_end = (char *)XMEM_END;
    __brkval = (char *)XMEM_START;

    for (uint8_t i = 0; i < XMEM_BANKS; i++) {
        _xmem_save_bank_state(&_bank_state[i]);
    }
#endif /* XMEM_HEAP_IN_XMEM */

    _current_bank = 0;
}
