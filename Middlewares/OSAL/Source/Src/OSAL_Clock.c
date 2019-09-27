/******************************************************************************
  Filename:       OSAL_Clock.c
  Revised:        $Date: 2014-06-30 16:38:56 -0700 (Mon, 30 Jun 2014) $
  Revision:       $Revision: 39297 $

  Description:    OSAL Clock definition and manipulation functions.
******************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "OSAL.h"

#include "OSAL_Timers.h"
#include "OSAL_Clock.h"

/*********************************************************************
 * MACROS
 */

#define	YearLength(yr)	((uint16)(IsLeapYear(yr) ? 366 : 365))

/*********************************************************************
 * CONSTANTS
 */

#define	BEGYEAR  2000     //  UTC started at 00:00:00 January 1, 2000

#define	DAY      86400UL  // 24 hours * 60 minutes * 60 seconds
                                  
/* Check Below for an explanation */
#define COUNTER_TICK320US 204775UL 

/* converted COUNTER_TICK320US from 320us ticks to ms */
#define COUNTER_ELAPSEDMS 65528UL   

/* 
 * Each tick is 320us so a value greater than 3 implies 
 * that atleast one millisecond has elapsed 320us*4 > 1 ms 
 */
#define TIMER_CLOCK_UPDATE 4 

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */
#ifndef USE_SYSTICK_IRQ
extern uint32 macMcuPrecisionCount(void);
#endif

/*  This function is used to divide a 31 bit dividend by a 16 bit
 *  divisor and return a packed 16 bit quotient and 16 bit
 *  remainder.
 *
 *  Note: This routine takes ~25.6us @32MHz. With C overhead, the
 *        time is ~32us.
 *
 *  dividend - 31 bit dividend.
 *  divisor - 16 bit divisor.
 *
 *  return - MSW divisor; LSW quotient
 */
#define CONVERT_320US_TO_MS_ELAPSED_REMAINDER( x, y, z ) st( \
  y += x / 25;                                               \
  z = x % 25;                                                \
)
  
#define CONVERT_MS_TO_S_ELAPSED_REMAINDER( x, y, z ) st(     \
  y += x / 1000;                                             \
  z = x % 1000;                                              \
)

/*********************************************************************
 * LOCAL VARIABLES
 */

#ifndef USE_SYSTICK_IRQ
static uint32 previousMacTimerTick = 0;
static uint16 remUsTicks = 0;
#endif

static uint32 timeMSec = 0;

// number of seconds since 0 hrs, 0 minutes, 0 seconds, on the
// 1st of January 2000 UTC
UTCTime OSAL_timeSeconds = 0;

/*********************************************************************
 * LOCAL FUNCTION PROTOTYPES
 */
static uint8 monthLength( uint8 lpyr, uint8 mon );

static void osalClockUpdate( uint32 elapsedMSec );

/*********************************************************************
 * FUNCTIONS
 *********************************************************************/

#ifndef USE_SYSTICK_IRQ
/*********************************************************************
 * @fn      osalTimeUpdate
 *
 * @brief   Uses the free running rollover count of the MAC backoff timer;
 *          this timer runs freely with a constant 320 usec interval.  The
 *          count of 320-usec ticks is converted to msecs and used to update
 *          the OSAL clock and Timers by invoking osalClockUpdate() and
 *          osalTimerUpdate().  This function is intended to be invoked
 *          from the background, not interrupt level.
 *
 * @param   None.
 *
 * @return  None.
 */
void osalTimeUpdate( void )
{
  /* Note that when ICall is in use the OSAL tick is not updated
   * in this fashion but rather through real OS timer tick. */
  halIntState_t intState;
  uint32 tmp;
  uint32 ticks320us;
  uint32 elapsedMSec = 0;

  HAL_ENTER_CRITICAL_SECTION(intState);
  // Get the free-running count of 320us timer ticks
  tmp = macMcuPrecisionCount();
  HAL_EXIT_CRITICAL_SECTION(intState);
  
  if ( tmp != previousMacTimerTick )
  {
    // Calculate the elapsed ticks of the free-running timer.
    ticks320us = (tmp - previousMacTimerTick) & 0xffffffffu;

    if (ticks320us >= TIMER_CLOCK_UPDATE )
    {
      // Store the MAC Timer tick count for the next time through this function.
      previousMacTimerTick = tmp;
      
      /*
       * remUsTicks can have a maximum value of 24 (Since remusTicks got by mod 
       * of 25). The value of COUNTER_TICK320US is a multiple of 25 and the 
       * quotient of  CONVERT_320US_TO_MS_ELAPSED_REMAINDER() does not exceed 
       * 0xFFFF or 16 bit.
       */
      while(ticks320us >= COUNTER_TICK320US)
      {
        ticks320us  -= COUNTER_TICK320US;
        elapsedMSec += COUNTER_ELAPSEDMS;
      }
    
      // update converted number with remaining ticks from loop and the
      // accumulated remainder from loop
      tmp = (ticks320us * 8) + remUsTicks;

      // Convert the 320 us ticks into milliseconds and a remainder
      CONVERT_320US_TO_MS_ELAPSED_REMAINDER( tmp, elapsedMSec, remUsTicks );
      
      // Update OSAL Clock and Timers
      osalClockUpdate( elapsedMSec );
      osalTimerUpdate( elapsedMSec );
    }
  }
}
#endif

/*********************************************************************
 * @fn      osalClockUpdate
 *
 * @brief   Updates the OSAL Clock time with elapsed milliseconds.
 *
 * @param   elapsedMSec - elapsed milliseconds
 *
 * @return  none
 */
static void osalClockUpdate( uint32 elapsedMSec )
{
  uint32 tmp;
  halIntState_t intState;
  
  HAL_ENTER_CRITICAL_SECTION(intState);
  // Add elapsed milliseconds to the saved millisecond portion of time
  timeMSec += elapsedMSec;

  // Roll up milliseconds to the number of seconds
  if ( timeMSec >= 1000 )
  {
    tmp = timeMSec;
    CONVERT_MS_TO_S_ELAPSED_REMAINDER(tmp, OSAL_timeSeconds, timeMSec);
  }
  HAL_EXIT_CRITICAL_SECTION(intState);
}

/*********************************************************************
 * @fn      osalAdjustTimer
 *
 * @brief   Updates the OSAL Clock and Timer with elapsed milliseconds.
 *
 * @param   MSec - elapsed milliseconds
 *
 * @return  none
 */
void osalAdjustTimer(uint32 Msec )
{
  /* Disable SysTick interrupts */ 
  SysTickIntDisable(); 
  
  osalClockUpdate(Msec);
  osalTimerUpdate(Msec);
  
  /* Enable SysTick interrupts */ 
  SysTickIntEnable(); 
}

/*********************************************************************
 * @fn      osal_setClock
 *
 * @brief   Set the new time.  This will only set the seconds portion
 *          of time and doesn't change the factional second counter.
 *
 * @param   newTime - number of seconds since 0 hrs, 0 minutes,
 *                    0 seconds, on the 1st of January 2000 UTC
 *
 * @return  none
 */
void osal_setClock( UTCTime newTime )
{
  HAL_CRITICAL_STATEMENT(OSAL_timeSeconds = newTime);
}

/*********************************************************************
 * @fn      osal_getClock
 *
 * @brief   Gets the current time.  This will only return the seconds
 *          portion of time and doesn't include the factional second
 *          counter.
 *
 * @param   none
 *
 * @return  number of seconds since 0 hrs, 0 minutes, 0 seconds,
 *          on the 1st of January 2000 UTC
 */
UTCTime osal_getClock( void )
{
  return ( OSAL_timeSeconds );
}

/*********************************************************************
 * @fn      osal_ConvertUTCTime
 *
 * @brief   Converts UTCTime to UTCTimeStruct
 *
 * @param   tm - pointer to breakdown struct
 *
 * @param   secTime - number of seconds since 0 hrs, 0 minutes,
 *          0 seconds, on the 1st of January 2000 UTC
 *
 * @return  none
 */
void osal_ConvertUTCTime( UTCTimeStruct *tm, UTCTime secTime )
{
  // calculate the time less than a day - hours, minutes, seconds
  {
    uint32 day = secTime % DAY;
    tm->seconds = day % 60UL;
    tm->minutes = (uint8)((day % 3600UL) / 60UL);
    tm->hour = (uint8)(day / 3600UL);
  }

  // Fill in the calendar - day, month, year
  {
    uint16 numDays = (uint16)(secTime / DAY);
    tm->year = BEGYEAR;
    while ( numDays >= YearLength( tm->year ) )
    {
      numDays -= YearLength( tm->year );
      tm->year++;
    }

    tm->month = 0;
    while ( numDays >= monthLength( IsLeapYear( tm->year ), tm->month ) )
    {
      numDays -= monthLength( IsLeapYear( tm->year ), tm->month );
      tm->month++;
    }

    tm->day = (uint8)numDays;
  }
}

/*********************************************************************
 * @fn      monthLength
 *
 * @param   lpyr - 1 for leap year, 0 if not
 *
 * @param   mon - 0 - 11 (jan - dec)
 *
 * @return  number of days in specified month
 */
static uint8 monthLength( uint8 lpyr, uint8 mon )
{
  uint8 days = 31;

	if ( mon == 1 ) // feb
  {
		days = ( 28 + lpyr );
  }
  else
  {
    if ( mon > 6 ) // aug-dec
    {
      mon--;
    }

    if ( mon & 1 )
    {
      days = 30;
    }
  }

	return ( days );
}

/*********************************************************************
 * @fn      osal_ConvertUTCSecs
 *
 * @brief   Converts a UTCTimeStruct to UTCTime
 *
 * @param   tm - pointer to provided struct
 *
 * @return  number of seconds since 00:00:00 on 01/01/2000 (UTC)
 */
UTCTime osal_ConvertUTCSecs( UTCTimeStruct *tm )
{
  uint32 seconds;

  /* Seconds for the partial day */
  seconds = (((tm->hour * 60UL) + tm->minutes) * 60UL) + tm->seconds;

  /* Account for previous complete days */
  {
    /* Start with complete days in current month */
    uint16 days = tm->day;

    /* Next, complete months in current year */
    {
      int8 month = tm->month;
      while ( --month >= 0 )
      {
        days += monthLength( IsLeapYear( tm->year ), month );
      }
    }

    /* Next, complete years before current year */
    {
      uint16 year = tm->year;
      while ( --year >= BEGYEAR )
      {
        days += YearLength( year );
      }
    }

    /* Add total seconds before partial day */
    seconds += (days * DAY);
  }

  return ( seconds );
}