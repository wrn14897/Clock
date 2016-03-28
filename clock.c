#include "timer.h"
#include "gpio.h"


#define DIGIT0 63    //0b00111111
#define DIGIT1 6     //0b00000110
#define DIGIT2 91    //0b01011011
#define DIGIT3 79    //0b01001111
#define DIGIT4 102   //0b01100110
#define DIGIT5 109   //0b01101101
#define DIGIT6 125   //0b01111101
#define DIGIT7 39    //0b00100111
#define DIGIT8 127   //0b01111111
#define DIGIT9 103   //0b01100111
#define PINNUM 54

//Define pin numbers connected with buttoms
#define BTM1PIN 4
#define BTM2PIN 5

/*
 * Returns the current system time in us.
 *
 * @return system time in microseconds
 */
unsigned int timer_get_time(void) {
  // TODO: Your code goes here.
    return *(unsigned int volatile*)(0x20003000 + 0x4);
}

/*
 * A simple busy loop that delays the program for `n` microseconds.
 *
 * @param n the number of microseconds to busy loop for
 */
void delay_us(unsigned int n) {
    unsigned int start = timer_get_time();
    while (timer_get_time() - start < n) { /* spin */ }
}

/*
 * Set GPIO pin number `pin` to the GPIO function `function`. Does not affect
 * other pins.
 *
 * @param pin the pin number to initialize
 * @param function the GPIO function to set for the pin
 */
void gpio_set_function(unsigned int pin, unsigned int function) {
  // TODO: Your code goes here.
    int assignValue = (function << (3*(pin % 10)));

    if (pin < GPIO_PIN10){
        *(int *)0x20200000 |= assignValue;;
    }
    else if (pin >= GPIO_PIN10 && pin < GPIO_PIN20){
        *(int *)0x20200004 |= assignValue;
    }
    else if (pin >= GPIO_PIN20 && pin < GPIO_PIN30){
        *(int *)0x20200008 |= assignValue;
    }
    else if (pin >= GPIO_PIN30 && pin < GPIO_PIN40){
        *(int *)0x2020000C |= assignValue;
    }
    else if (pin >= GPIO_PIN40 && pin < GPIO_PIN50){
        *(int *)0x20200010 |= assignValue;
    }
    else{
        *(int *)0x20200014 |= assignValue;
    } 
}

/*
 * Sets GPIO pin number `pin` to the value `value`. The GPIO pin should be set
 * the output mode for this call to function correctly.
 *
 * @param pin the pin number to set or clear
 * @param value 1 if the pin should be set, 0 otherwise
 */
void gpio_write(unsigned int pin, unsigned int value) {
    // TODO: Your code goes here.
    if (pin < GPIO_PIN32){
        if (value == 1){
            *(int *)0x2020001C = (1 << pin);
        }
        else{
            *(int *)0x20200028 = (1 << pin);
        }
    }
    else{
        if (value == 1){
            *(int *)0x20200020 = (1 << (pin - 32));
        }
        else{
            *(int *)0x2020002C = (1 << (pin - 32));
        }
    }
}
/*
 * Read the value of pin (true -> high, false -> low)
 */
unsigned int gpio_read(unsigned int pin){
    if (pin < GPIO_PIN32){
        gpio_set_function(pin, GPIO_FUNC_INPUT);
        if( *(int*)0x20200034 & (1 << pin) ){
            return 1;
        };
    }
    return 0;
}


/*
 * Write gpio depends on specific code pattern(declare on the top of program)
 */
void gpio_write_code(unsigned int code){
    unsigned int startPin = 20;

    while (code){
        gpio_write(startPin, (code & 1));
        code = code >> 1;
        startPin ++;
    }
    //set left segments to 0
    while (startPin < 28){
        gpio_write(startPin, 0);
        startPin ++;
    }
}

/*
 * Clear all segments
 */
void clearDigit(unsigned int position){
    switch(position){
        case 0:
            gpio_write(10, 0);
            break;
        case 1:
            gpio_write(11, 0);
            break;
        case 2:
            gpio_write(12, 0);
            break;
        case 3:
            gpio_write(13, 0);
            break;
        default:
            gpio_write(13, 0);
            break;
    }
}



/*
 *
 *Display a numerical digit 
 *
 *@param digit -> 0 ~ 9, 
 *@param position -> 0 ~ 3 (left to right)
 */ 
void displayNumDigit(unsigned int digit, unsigned int position){
    //GPIO output configure 
    if (position < 4){
        //Control digit position
        gpio_set_function( (10+position), GPIO_FUNC_OUTPUT );
        gpio_write( (10+position), 1 );

        //set pin20 ~ pin27 to output mode
        for(int i = 20; i < 28; i++){
            gpio_set_function(i, GPIO_FUNC_OUTPUT);
        }
    }
    
    switch(digit){
        case 0:
            gpio_write_code(DIGIT0);
            break;
        case 1:
            gpio_write_code(DIGIT1);
            break;
        case 2:
            gpio_write_code(DIGIT2);
            break;
        case 3:
            gpio_write_code(DIGIT3);
            break;
        case 4:
            gpio_write_code(DIGIT4);
            break;
        case 5:
            gpio_write_code(DIGIT5);
            break;
        case 6:
            gpio_write_code(DIGIT6);
            break;
        case 7:
            gpio_write_code(DIGIT7);
            break;
        case 8:
            gpio_write_code(DIGIT8);
            break;
        case 9:
            gpio_write_code(DIGIT9);
            break;
        default:
            gpio_write( (10+position), 0 );
            break;
    }
    

}

/*
 * This function will loop through all four digits 
 */
void loopThroughAllDigits(){
    for (int j = 0; j < 4; j++){
        clearDigit(j-1);
        for (int i = 0; i < 10; i++){
            displayNumDigit(i, j);
            delay_us(1000000); //delay 1 sec
        }
        delay_us(2500); //delay 2.5 ms 
    }
}


/*
 * Transfer milliseconds to hours, minutes and seconds
 */
unsigned int *TransferTimeFormat(unsigned int milliseconds){
    static unsigned int t[3];
    t[0] = milliseconds/(3600*1000); //hr
    t[1] = (milliseconds - t[0]*3600000)/60000; //min
    t[2] = (milliseconds - t[0]*3600000 - t[1]*60000)/1000; //sec

    //Upper bound for hours ?? 24 hr/day
    t[0] %= 24;
    return t;
}

/*
 * Update time and digits array content based on "mode format"
 * @param digits -> an array with 4 elemts 
 * @param startT -> start time in milliseconds
 * @param mode   -> "0" -> (hr, min), "1" -> (min, sec)
 * @param timeoffset -> ms
 * @param currentT   -> ms
 */
void updateTime(unsigned int *digits, unsigned int timeoffset, unsigned int mode){
    unsigned int  currentT  = (timer_get_time()/1000 + timeoffset); //ms
    unsigned int *startTBar = TransferTimeFormat(currentT);
    if (mode == 0){
        digits[0] = startTBar[0] / 10;
        digits[1] = startTBar[0] % 10;
        digits[2] = startTBar[1] / 10;
        digits[3] = startTBar[1] % 10;
    }
    else{
        digits[0] = startTBar[1] / 10;
        digits[1] = startTBar[1] % 10;
        digits[2] = startTBar[2] / 10;
        digits[3] = startTBar[2] % 10;
    } 
}



/*
 * Clock implementation
 * @param version -> "0" (hr, min), "1"(min, sec)
 */
void clock(unsigned int version){
    //Use an array to save time data
    static unsigned int digits[4] = {0};
    unsigned int timeoffset = 0;
    //Here we need another vars to take care of user inputs 
    //(what if user press the buttom for a long time ??)
    unsigned int btm1 = 0;
    unsigned int btm2 = 0;


    //Display defalut clock
    while (1){
        //Check user input
        if (version){
            //Increment sec
            if (gpio_read(BTM1PIN)){
                if (btm1 == 0){
                    btm1 = 1;
                    timeoffset += 1000;
                } 
            }
            else{
                btm1 = 0;
            }
            //Increment min
            if (gpio_read(BTM2PIN)){
                if (btm2 == 0){
                    btm2 = 1;
                    timeoffset += 60000;
                } 
            }
            else{
                btm2 = 0;
            }
        }
        else{
            //Increment min
            if (gpio_read(BTM1PIN)){
                if (btm1 == 0){
                    btm1 = 1;
                    timeoffset += 60000;
                } 
            }
            else{
                btm1 = 0;
            }
            //Increment hr
            if (gpio_read(BTM2PIN)){
                if (btm2 == 0){
                    btm2 = 1;
                    timeoffset += 3600000;
                } 
            }
            else{
                btm2 = 0;
            }
        }
        
        
        updateTime(digits, timeoffset, version);

        for (int digitInd = 0; digitInd < 4; digitInd++){
            clearDigit(digitInd-1);
            displayNumDigit(digits[digitInd], digitInd);
            delay_us(2500);
        }
    }
}

/*
 * Test buttom function
 */
void testButtom(unsigned int pin){
    while(1){
        if(gpio_read(pin)){
            displayNumDigit(1, 0);
        }
        else{
            displayNumDigit(0, 0);
        }
    }
}



void main(void) {
  // TODO: Your code goes here.
  clock(1);
  
  //testButtom(BTM1PIN);
}
