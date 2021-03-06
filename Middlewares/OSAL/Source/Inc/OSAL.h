/******************************************************************************
  Filename:     OSAL.h
  Revised:      $Date: 2014-06-30 16:38:56 -0700 (Mon, 30 Jun 2014) $
  Revision:     $Revision: 39297 $

  Description:  This API allows the software components in the Z-Stack to be
                written independently of the specifics of the operating system,
                kernel, or tasking environment (including control loops or
                connect-to-interrupt systems).
******************************************************************************/

#ifndef OSAL_H
#define OSAL_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "OSAL_Comdef.h"

/*********************************************************************
 * MACROS
 */
#if ( UINT_MAX == 65535 ) /* 8-bit and 16-bit devices */
  #define osal_offsetof(type, member) ((uint16_t) &(((type *) 0)->member))
#else /* 32-bit devices */
  #define osal_offsetof(type, member) ((uint32_t) &(((type *) 0)->member))
#endif

#define OSAL_MSG_NEXT(msg_ptr)      ((osal_msg_hdr_t *) (msg_ptr) - 1)->next

#define OSAL_MSG_Q_INIT(q_ptr)      *(q_ptr) = NULL

#define OSAL_MSG_Q_EMPTY(q_ptr)     (*(q_ptr) == NULL)

#define OSAL_MSG_Q_HEAD(q_ptr)      (*(q_ptr))

#define OSAL_MSG_LEN(msg_ptr)       ((osal_msg_hdr_t *) (msg_ptr) - 1)->len

#define OSAL_MSG_ID(msg_ptr)        ((osal_msg_hdr_t *) (msg_ptr) - 1)->dest_id

/*********************************************************************
 * CONSTANTS
 */

/*** Interrupts ***/
#define INTS_ALL    0xFF

/*********************************************************************
 * TYPEDEFS
 */

typedef struct
{
  void   *next;
  uint16_t len;
  uint8_t  dest_id;
} osal_msg_hdr_t;

typedef struct
{
  uint8_t  event;
  uint8_t  status;
} osal_event_hdr_t;

typedef void * osal_msg_q_t;

typedef struct mutex_struct
{
 uint32_t mutex_value;
 struct mutex_struct *next_mutex;
}osal_mutex_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * FUNCTIONS
 */

/*** Helper Functions ***/

  /*
   * Convert an interger to an ascii string
   */
  extern uint32_t init_rand(uint32_t seed);

  /*
   * Convert an interger to an ascii string
   */
  extern uint32_t _rand(void);

  /*
   * Convert an interger to an ascii string
   */
  extern int32_t rand_range(int32_t min, int32_t max);

  /*
   * Convert an interger to an ascii string
   */
  extern char* osal_strcat(char* dst, const char* src);

  /*
   * Convert an interger to an ascii string
   */
  extern char* osal_strcpy(char* dst, const char* src);

  /*
   * Convert an interger to an ascii string
   */
  extern size_t osal_strnlen(const char* str, size_t maxlen);

  /*
   * Convert an interger to an ascii string
   */
  extern size_t osal_strncpy_m(char *destStr, size_t destSize, int nStrings, ...);

  /*
   * Convert an interger to an ascii string
   */
  extern char* osal_strncpy(char* dst, const char* src, size_t maxlen);

  /*
   * String Length
   */
  extern int osal_strlen( const char* pString );

  /*
   * Memory copy
   */
  extern void* osal_memcpy(void* dst, const void GENERIC* src, unsigned int len);

  /*
   * Memory Duplicate - allocates and copies
   */
  extern void *osal_memdup( const void GENERIC *src, unsigned int len );

  /*
   * Reverse Memory copy
   */
  extern void* osal_revmemcpy(void* dst, const void GENERIC* src, unsigned int len);

  /*
   * Memory compare
   */
  extern uint8_t osal_memcmp( const void GENERIC *src1, const void GENERIC *src2, unsigned int len );

  /*
   * Memory set
   */
  extern void *osal_memset( void *dest, uint8_t value, int len );

  /*
   * Build a uint16_t out of 2 bytes (0 then 1).
   */
  extern uint16_t osal_build_uint16( uint8_t *swapped );

  /*
   * Build a uint32_t out of sequential bytes.
   */
  extern uint32_t osal_build_uint32( uint8_t *swapped, uint8_t len );

  /*
   * Convert long to ascii string
   */
  extern uint8_t *osal_ltoa( uint32_t l, uint8_t * buf, uint8_t radix );

  /*
   * convert a string int to a long unsigned.
   */
  extern long osal_atol(const char *s);

  /*
   * Random number generator
   */
  extern uint32_t osal_rand(void);

  /*
   * Buffer an uint32_t value - LSB first.
   */
  extern uint8_t* osal_buffer_uint32( uint8_t *buf, uint32_t val );

  /*
   * Buffer an uint32_t value - LSB first
   */
  extern uint8_t* osal_buffer_uint24( uint8_t *buf, uint32_t val );

  /*
   * Is all of the array elements set to a value?
   */
  extern uint8_t osal_isbufset( uint8_t *buf, uint8_t val, uint8_t len );

  /*
   * Convert an interger to an ascii string
   */
  extern void osal_itoa( uint16_t num, uint8_t *buf, uint8_t radix );

/*** Mutex Management ***/

  /*
   * Task Message Allocation
   */
  osal_mutex_t* osalMutexCreate(void);
  /*
   * Task Message Allocation
   */
  void osalMutexDelete(osal_mutex_t** mutex);
  /*
   * Task Message Allocation
   */
  void osalMutexTake(osal_mutex_t** mutex,uint32_t mutex_overtime);
  /*
   * Task Message Allocation
   */
  uint32_t osalMutexCheck(osal_mutex_t* mutex);
  /*
   * Task Message Allocation
   */
  void osalMutexRelease(osal_mutex_t** mutex);
  /*
   * Task Message Allocation
   */
  void osalMutexUpdate( uint32_t mutexTime );

/*** Message Management ***/

  /*
   * Task Message Allocation
   */
  extern uint8_t * osal_msg_allocate(uint16_t len );

  /*
   * Task Message Deallocation
   */
  extern uint8_t osal_msg_deallocate( uint8_t *msg_ptr );

  /*
   * Send a Task Message
   */
  extern uint8_t osal_msg_send( uint8_t destination_task, uint8_t *msg_ptr );

  /*
   * Push a Task Message to head of queue
   */
  extern uint8_t osal_msg_push_front( uint8_t destination_task, uint8_t *msg_ptr );

  /*
   * Receive a Task Message
   */
  extern uint8_t *osal_msg_receive( uint8_t task_id );

  /*
   * Find in place a matching Task Message / Event.
   */
  extern osal_event_hdr_t *osal_msg_find(uint8_t task_id, uint8_t event);

  /*
   * Count the number of queued OSAL messages matching Task ID / Event.
   */
  extern uint8_t osal_msg_count(uint8_t task_id, uint8_t event);

  /*
   * Enqueue a Task Message
   */
  extern void osal_msg_enqueue( osal_msg_q_t *q_ptr, void *msg_ptr );

  /*
   * Enqueue a Task Message Up to Max
   */
  extern uint8_t osal_msg_enqueue_max( osal_msg_q_t *q_ptr, void *msg_ptr, uint8_t max );

  /*
   * Dequeue a Task Message
   */
  extern void *osal_msg_dequeue( osal_msg_q_t *q_ptr );

  /*
   * Push a Task Message to head of queue
   */
  extern void osal_msg_push( osal_msg_q_t *q_ptr, void *msg_ptr );

  /*
   * Extract and remove a Task Message from queue
   */
  extern void osal_msg_extract( osal_msg_q_t *q_ptr, void *msg_ptr, void *prev_ptr );

/*** Task Synchronization  ***/

  /*
   * Set a Task Event
   */
  extern uint8_t osal_set_event( uint8_t task_id, uint16_t event_flag );

  /*
   * Clear a Task Event
   */
  extern uint8_t osal_clear_event( uint8_t task_id, uint16_t event_flag );


  /*** Interrupt Management  ***/

  /*
   * Register Interrupt Service Routine (ISR)
   */
  extern uint8_t osal_isr_register(uint8_t interrupt_id, void (*isr_ptr)(uint8_t*));

  /*
   * Enable Interrupt
   */
  extern uint8_t osal_int_enable( uint8_t interrupt_id );

  /*
   * Disable Interrupt
   */
  extern uint8_t osal_int_disable( uint8_t interrupt_id );


/*** Task Management  ***/

  /*
   * Initialize the Task System
   */
  extern uint8_t osal_init_system( void );

  /*
   * System Processing Loop
   */
  extern void osal_start_system( void );

  /*
   * One Pass Throu the OSAL Processing Loop
   */
  extern void osal_run_system( void );

  /*
   * Get the active task ID
   */
  extern uint8_t osal_self( void );


/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* OSAL_H */
