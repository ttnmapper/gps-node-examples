//#include "mbed.h"
#include <stdint.h>

/**
 * Class to make wake up a microcontroller from deepsleep using a low-power timer. 
 *
 * @code
 * // Depending on the LED connections either the LED is off the 2 seconds
 * // the target spends in deepsleep(), and on for the other second. Or it is inverted 
 * 
 * #include "mbed.h"
 * #include "WakeUp.h"
 * 
 * DigitalOut myled(LED1);
 * 
 * int main() {
 *     wait(5);
 *
 *     //The low-power oscillator can be quite inaccurate on some targets
 *     //this function calibrates it against the main clock
 *     WakeUp::calibrate();
 *    
 *     while(1) {
 *         //Set LED to zero
 *         myled = 0;
 *         
 *         //Set wakeup time for 2 seconds
 *         WakeUp::set_ms(2000);
 *         
 *         //Enter deepsleep, the program won't go beyond this point until it is woken up
 *         deepsleep();
 *         
 *         //Set LED for 1 second to one
 *         myled = 1;
 *         wait(1);
 *     }
 * }
 * @endcode
 */

class WakeUp
{
public:
    /**
    * Set the timeout
    *
    * @param s required time in seconds
    */
    static void set(uint32_t s) {
        set_ms(1000 * s);
    }
    
    /**
    * Set the timeout
    *
    * @param ms required time in milliseconds
    */
    static void set_ms(uint32_t ms);
    
    /**
    * Attach a function to be called after timeout
    *
    * This is optional, if you just want to wake up you 
    * do not need to attach a function.
    *
    * Important: Many targets will run the wake-up routine
    * at reduced clock speed, afterwards clock speed is restored.
    * This means that clock speed dependent functions, such as printf
    * might end up distorted.
    *
    * @code
    * // Attaching regular function
    * WakeUp::attach(&yourFunc);
    * // Attaching member function inside another library    
    * WakeUp::attach(callback(this, &YourLib::yourLibFunction));    
    * @endcode
    *
    * It uses the new Callback system to attach functions.
    *
    * @param *function function to call
    */
    //static void attach(Callback<void()> function) {
    //    callback = function;
    //}
    
    /**
    * Calibrate the timer
    *
    * Some of the low-power timers have very bad accuracy.
    * This function calibrates it against the main timer.
    *
    * Warning: Blocks for 100ms!
    */
    static void calibrate(void);


private:
    //static Callback<void()> callback;
    static void irq_handler(void);
    static float cycles_per_ms;
};

