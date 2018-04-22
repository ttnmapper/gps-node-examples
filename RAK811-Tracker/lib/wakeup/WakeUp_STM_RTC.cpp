#ifdef TARGET_STM

#include "WakeUp.h"
//#include "rtc_api.h"
#include "stm32l1xx_hal_rtc.h"
#include "stm32l1xx_hal_pwr.h"

/*
https://community.st.com/thread/45343-how-dose-stm32l151ccu6-enter-stop-mode
// Enter Stop Mode 
HAL_SuspendTick();
HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
HAL_ResumeTick();
*/

#define BYTE2BCD(byte)      ((byte % 10) | ((byte / 10) << 4))

//Most things are pretty similar between the different STM targets.
//Only the IRQ number the alarm is connected to differs. Any errors
//with RTC_IRQn/RTC_Alarm_IRQn in them are related to this
#if defined(TARGET_M4) || defined(TARGET_M3)
#define RTC_IRQ     RTC_Alarm_IRQn               
#else
#define RTC_IRQ     RTC_IRQn
#endif

// Some things to handle Disco L476VG (and similar ones)
#if defined(TARGET_STM32L4)
#define IMR     IMR1
#define EMR     EMR1
#define RTSR    RTSR1
#define FTSR    FTSR2
#define PR      PR1
#endif

//Disabling the Backup Powerdomain does not seem to work nicely anymore if you want to use other RTC functions afterwards.
//For now I have disabled it in code, if you find WakeUp increases your powerconsumption, try enabling it again (code is still there, just commented)

//Callback<void()> WakeUp::callback;

void WakeUp::set_ms(uint32_t ms)
{        
    if (!rtc_isenabled()) {      //Make sure RTC is running
        HAL_RTC_Init();
        HAL_Delay(1/*250*/);            //The f401 seems to want a delay after init
    }
    
    //PWR->CR |= PWR_CR_DBP;      //Enable power domain
    RTC->WPR = 0xCA;            //Disable RTC write protection
    RTC->WPR = 0x53;
    
    //Alarm must be disabled to change anything
    RTC->CR &= ~RTC_CR_ALRAE;
    while(!(RTC->ISR & RTC_ISR_ALRAWF));
    
    if (ms == 0) {              //Just disable alarm
        //PWR->CR &= ~PWR_CR_DBP; //Disable power domain
        RTC->WPR = 0xFF;        //Enable RTC write protection
        return;
    }
    
    //RTC prescaler + calculate how many sub-seconds should be added
    uint32_t prescaler = (RTC->PRER & 0x7FFF) + 1;
    uint32_t subsecsadd = ((ms % 1000) * prescaler) / 1000;
    
    if ((ms < 1000) && (subsecsadd < 2))
        subsecsadd = 2;                             //At least 2 subsecs delay to be sure interrupt is called
    
    __disable_irq();                                //At this point we don't want IRQs anymore
    
    //Get current time
    uint32_t subsecs = RTC->SSR;
    time_t secs = rtc_read();
    
    //Calculate alarm values
    //Subseconds is countdown, so substract the 'added' sub-seconds and prevent underflow
    if (subsecs < subsecsadd) {
        subsecs += prescaler;
        secs++;
    }
    subsecs -= subsecsadd;
    
    //Set seconds correctly
    secs += ms / 1000;
    struct tm *timeinfo = localtime(&secs);
    
    //Enable rising edge EXTI interrupt of the RTC
    EXTI->IMR |= RTC_EXTI_LINE_ALARM_EVENT;
    EXTI->EMR &= ~RTC_EXTI_LINE_ALARM_EVENT;
    EXTI->RTSR |= RTC_EXTI_LINE_ALARM_EVENT;
    EXTI->FTSR &= ~RTC_EXTI_LINE_ALARM_EVENT;
    
    //Calculate alarm register values
    uint32_t alarmreg = 0;
    alarmreg |= BYTE2BCD(timeinfo->tm_sec)  << 0;
    alarmreg |= BYTE2BCD(timeinfo->tm_min)  << 8;
    alarmreg |= BYTE2BCD(timeinfo->tm_hour) << 16;
    alarmreg |= BYTE2BCD(timeinfo->tm_mday) << 24;
    
    //Enable RTC interrupt
    RTC->ALRMAR = alarmreg;
    RTC->ALRMASSR = subsecs | RTC_ALRMASSR_MASKSS;      //Mask no subseconds
    RTC->CR |= RTC_CR_ALRAE | RTC_CR_ALRAIE;            //Enable Alarm
    
    RTC->WPR = 0xFF;        //Enable RTC write protection
    //PWR->CR &= ~PWR_CR_DBP; //Disable power domain
    
    __enable_irq();         //Alarm is set, so irqs can be enabled again
    
    //Enable everything else
    NVIC_SetVector(RTC_IRQ, (uint32_t)WakeUp::irq_handler);
    NVIC_EnableIRQ(RTC_IRQ);    
}


void WakeUp::irq_handler(void)
{
    //Clear RTC + EXTI interrupt flags
    //PWR->CR |= PWR_CR_DBP;      //Enable power domain
    RTC->ISR &= ~RTC_ISR_ALRAF;
    RTC->WPR = 0xCA;            //Disable RTC write protection
    RTC->WPR = 0x53;
    RTC->CR &= ~RTC_CR_ALRAE;
    RTC->WPR = 0xFF;        //Enable RTC write protection
    EXTI->PR = RTC_EXTI_LINE_ALARM_EVENT;
    //PWR->CR &= ~PWR_CR_DBP;     //Disable power domain 
    //callback.call();
}

void WakeUp::calibrate(void)
{
    //RTC, we assume it is accurate enough without calibration
}


#endif
