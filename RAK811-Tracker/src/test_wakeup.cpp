#include "WakeUp.h"
#include "stm32l1xx_ll_pwr.h"
#include "stm32l1xx_hal_pwr.h"

// https://community.st.com/thread/46727-no-interrupts-in-low-power-stm32l151

extern "C" void  test_wakeup() {
        //Set wakeup time for 2 seconds
        WakeUp::set_ms(2000);
        
        //Enter deepsleep, the program won't go beyond this point until it is woken up
        HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFE);
        //LL_PWR_SetPowerMode(LL_PWR_MODE_STANDBY);
        //deepsleep();
}