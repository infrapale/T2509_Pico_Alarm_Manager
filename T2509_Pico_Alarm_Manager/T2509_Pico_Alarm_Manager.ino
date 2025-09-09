
#include "Arduino.h"
#include <Wire.h>
#include "main.h"
#include "io.h"
#include "atask.h"
#include "uart.h"
#include "pir.h"

    // <A1A1?->
    // <A1W1?->
    // <A1D1?->

void debug_print_task(void);

// Task definitions (may also be defined in modules)
atask_st debug_print_handle        = {"Debug Print    ", 5000,0, 0, 255, 0, 1, debug_print_task};



uart_st         *uartp;

void setup() {
    SerialGw.setTX(PIN_SERIAL_0_TX);   // UART0
    SerialGw.setRX(PIN_SERIAL_0_RX);
    Serial.begin(9600);
    SerialGw.begin(9600);

    delay(3000);
    // while (!Serial);
    Serial.printf("%s: %s %s\n",APP_NAME,__DATE__,__TIME__);
    uartp = uart_get_data_ptr();

    atask_initialize();
    atask_add_new(&debug_print_handle);
    
    uart_initialize();
    pir_initialize();


}

void loop() {
    atask_run();  

    // Serial1.println("<A1D1?->");
    // Serial.println("<A1D1?->");
    // delay(200);
    // if (SerialGw.available())
    // {
    //     uartp->rx.msg.str = SerialGw.readStringUntil('\n');
    //     if (uartp->rx.msg.str.length()> 0)
    //     {
    //         uartp->rx.msg.avail = true;
    //         //uart.rx.str.remove(uart.rx.str.length()-1);
    //     }
    //     Serial.print("rx is available: ");
    //     Serial.println(uartp->rx.msg.str);
    // } 

    // // while(!uart_read_uart()) {
    // //   delay(10);
    // // };
    // delay(500);


}

void debug_print_task(void)
{
    #ifdef DEBUG_PRINT
    atask_print_status(true);
    #endif
}


