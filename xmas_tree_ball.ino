#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

#define DELAY 5 //multiple of 16 ms
#define LINE1 2
#define LINE2 3
#define LINE3 4
#define H HIGH //cannot use enum due to memory constraints
#define L LOW
#define Z 2
#define NUM_LED 11
#define NUM_LINES 3

int lines[NUM_LINES] = {LINE1, LINE2, LINE3};
byte leds[NUM_LED][NUM_LINES] = {{Z, Z, Z}, 
                                 {H, L, Z}, 
                                 {L, H, Z}, 
                                 {Z, H, L}, 
                                 {Z, L, H}, 
                                 {H, Z, L}, 
                                 {L, Z, H}, 
                                 {H, L, L}, 
                                 {H, H, L}, 
                                 {L, H, H}, 
                                 {L, L, H}};

void setup() {
    ADCSRA &= ~(1<<ADEN); //turn off ADC
    ACSR |= _BV(ACD);     // disable analog comparator
    init_pin();
}

void init_pin() {
    for(int i = 0; i < NUM_LINES; i++)
      pinMode(lines[i], INPUT);
}

void set_led(int num) {
    if (num >=0 && num < NUM_LED){
        init_pin();
        for(int i = 0; i < NUM_LINES; i++)
            if (leds[num][i] != Z) {
              pinMode(lines[i], OUTPUT);
              digitalWrite(lines[i], leds[num][i]);              
            }
    }
}

void goSleep() {
    watchdogSetup();
    power_timer0_disable();
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_mode();
    wdt_disable();
    power_timer0_enable();
}

void watchdogSetup() {
    /*
    WDP3 WDP2 WDP1 WDP0
    0 0 0 0 2K (2048) cycles 16 ms
    0 0 0 1 4K (4096) cycles 32 ms
    0 0 1 0 8K (8192) cycles 64 ms
    0 0 1 1 16K (16384) cycles 0.125 s
    0 1 0 0 32K (32768) cycles 0.25 s
    0 1 0 1 64K (65536) cycles 0.5 s
    0 1 1 0 128K (131072) cycles 1.0 s
    0 1 1 1 256K (262144) cycles 2.0 s
    1 0 0 0 512K (524288) cycles 4.0 s
    1001 1024K (1048576) cycles 8.0 s
    */
  
    WDTCR |= (0<<WDP3 )|(0<<WDP2 )|(0<<WDP1)|(0<<WDP0); // 16ms
    WDTCR |= (1<<WDTIE);
    WDTCR |= (0<<WDE);
    sei(); // Enable global interrupts
}

void waitSleep(int times) {
    while (times) {
        goSleep();
        times--;
    }
}

ISR(WDT_vect) {
}

void loop() {
    for(int i = 0; i < NUM_LED; i++) {
      set_led(i);
      waitSleep(DELAY);
    }
    set_led(0);
    waitSleep(5 * DELAY);
}
