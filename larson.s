/*
 * LARSON SCANNER
 *
 * Currently this code is the blink code copied from lab 1.
 * Modify the code to implement the larson scanner for assignment 1.
 *
 * Make sure to use GPIO pins 20-23 (or 20-27) for your scanner.
 */

// configure GPIO 20, 21, 22, 23 for output (0x20200008 group)
ldr r0, =0x20200008
mov r1, #1
add r1, #(1<<3)
add r1, #(1<<6)
add r1, #(1<<9)
add r1, #(1<<12)
add r1, #(1<<15)
add r1, #(1<<18)
add r1, #(1<<21)
str r1, [r0]

ldr r3, =0x2020001C
ldr r4, =0x20200028

loop:
    mov r2, #(1<<20)
    //Counter 
    mov r5, #8


    /*****************************/
    /** Scan from left to right **/
    /*****************************/
    loop1: 
        str r2, [r3] //Turn on central LED

        /** Implement PWM to dim LED **/

        mov r6, #0x00F0000 //Total delay time
        pwmLoop1:
            /** Left LED **/
            lsr r2, #1   
            //Different duty cycle
            mov  r7, #3
            mov  r8, #1
            str r2, [r3] //Turn on 
            wait1:
                subs r7, #1
            bne wait1
            str r2, [r4] //Turn off 
            wait2:
                subs r8, #1
            bne wait2


            /** Right LED **/
            lsl r2, #2
            //Different duty cycle
            mov  r7, #3
            mov  r8, #1

            str r2, [r3] //Turn on
            
            wait3:
                subs r7, #1
            bne wait3
            str r2, [r4] //Turn off 

            wait4:
                subs r8, #1
            bne wait4
            lsr r2, #1

            subs r6, #8
            cmp  r6, #0
        bne pwmLoop1

        str r2, [r4] //Turn off central LED
        
        lsl  r2, #1 //Move to next LED
        subs r5, #1
        cmp  r5, #0
    bne loop1
        
    /*****************************/
    /** Scan from right to left **/ 
    /*****************************/
    lsr r2, #2
    mov r5, #6
    loop2:
        str r2, [r3] //Turn on central LED

        /** Implement PWM to dim LED **/

        mov r6, #0x00F0000 //Total delay time
        pwmLoop2:
            /** Left LED **/
            lsr r2, #1   
            //Different duty cycle
            mov  r7, #3
            mov  r8, #1

            str r2, [r3] //Turn on
            wait5:
                subs r7, #1
            bne wait5
            str r2, [r4] //Turn off
            wait6:
                subs r8, #1
            bne wait6


            /** Right LED **/
            lsl r2, #2
            //Different duty cycle
            mov  r7, #3
            mov  r8, #1

            str r2, [r3] //Turn on
            wait7:
                subs r7, #1
            bne wait7
            str r2, [r4] //Turn off
            wait8:
                subs r8, #1
            bne wait8
            lsr r2, #1

            subs r6, #8
            cmp  r6, #0
        bne pwmLoop2

        str r2, [r4] //Turn off central LED
        
        lsr  r2, #1 //Move to next LED
        subs r5, #1
        cmp  r5, #0
    bne loop2
b loop




