#include "main.h"
#include "atask.h"
#include "uart.h"
#include "json.h"

#include "io.h"
#include "pir.h"

typedef struct
{
    uint32_t interval;
    uint32_t active;
    uint32_t timeout;
} pir_test_st;

pir_st  pir[NBR_OF_PIR] = 
{
    // zone name state prev_state status event prev_active timeout;
    {"Piha", "PIR1",    0,  0,  0,  0},
    {"Piha", "PIR2",    0,  0,  0,  0},
    {"Auto", "PIR1",    0,  0,  0,  0},
    {"Jarv", "PIR1",    0,  0,  0,  0},
};

pir_test_st pir_test[NBR_OF_PIR] = 
{
    { 33000, 5000, 0},
    { 60000, 4000, 0},
};

typedef struct
{
  uint8_t   sensor_indx;
  uart_st *uartp;
  bool      alarm_on;
} pir_ctrl_st;

void pir_task(void);
void pir_alarm_task(void);

atask_st pir_handle         = {"PIR Task       ", 100,0, 0, 255, 0, 1, pir_task};
atask_st pir_alarm_handle   = {"PIR Alarm Task ", 500,0, 0, 255, 0, 1, pir_alarm_task};

pir_ctrl_st pir_ctrl;

void pir_initialize(void)
{
    pir_ctrl.sensor_indx = 0;
    pir_ctrl.uartp = uart_get_data_ptr();
    atask_add_new(&pir_handle);
    atask_add_new(&pir_alarm_handle);
}

void pir_update(uart_node_st *node)
{
    uint8_t pindx = 0;
    bool    done = false;
    while((pindx < NBR_OF_PIR) && !done)
    {
        if((node->zone == pir[pindx].zone) &&
            (node->name == pir[pindx].name))
        {
            if(node->value == "H") pir[pindx].event = PIR_STATUS_ACTIVE;
            else pir[pindx].event = PIR_STATUS_INACTIVE;
            Serial.printf("pir_update %d = %d\n", pindx, pir[pindx].event);
            done = true;
        }
        pindx++;
    }
}

// String)UART_VALUE_HIGH

void pir_set_status(uint8_t indx, uint8_t new_status)
{
    if (indx < NBR_OF_PIR)
    {
        pir[indx].event = new_status;
    }
    else
    {
        Serial.print(F("Incorrect PIR index: ")); Serial.println(indx,HEX);
    }
} 

// void pir_send_radio_msg(pir_st *pirp)
// {
//     pir_ctrl.uartp->node.name = pirp->name;
//     //Serial.print(F("Name:")); Serial.println(pir_ctrl.uartp->node.name);
//     pir_ctrl.uartp->node.zone = pirp->zone;
//     switch(pirp->event)
//     {
//         case PIR_STATUS_ACTIVE:
//             pir_ctrl.uartp->node.value = 'H';
//             break;
//         case PIR_STATUS_INACTIVE:
//             pir_ctrl.uartp->node.value = 'L';
//             break;
//         default:
//             pir_ctrl.uartp->node.value = 'X';
//             break;
//     }
//     pir_ctrl.uartp->node.remark= "-";
//     //Serial.print(F("Zone:")); Serial.println(pir_ctrl.uartp->node.zone);

//     rfm_send_msg_st *send_p = rfm_send_get_data_ptr();
//     json_convert_uart_node_to_json(send_p->radio_msg, pir_ctrl.uartp);
//     //Serial.println(send_p->radio_msg);
//     rfm_send_radiate_msg(send_p->radio_msg);
// }

void pir_state_machine(pir_st *pirp)
{
    switch(pirp->state)
    {
        case 0:  // start
            if(pirp->event != PIR_STATUS_UNDEFINED)
            pirp->state = 10;
            break;
        case 10:  // not activated
            //pirp->prev_active = pirp->event;
            //pir_send_radio_msg(pirp);
            if(pirp->event == PIR_STATUS_ACTIVE)
            {
                pirp->status = PIR_STATUS_ACTIVE;
                pirp->timeout = millis() + 20000;
                pirp->state = 20;
            }
            // else 
            // {
            //     pirp->status = PIR_STATUS_INACTIVE;
            //     pirp->timeout = millis() + 1000;
            //     pirp->state = 50;
            // }
            break;
        case 20:
            if(millis() > pirp->timeout ) {
                //pirp->status = PIR_STATUS_INACTIVE;
                pirp->state = 25;
                pirp->timeout = millis() + 20000;
            }
            break;
        case 25:
            if(pirp->event == PIR_STATUS_INACTIVE)
            {
                pirp->status = PIR_STATUS_INACTIVE;
                pirp->timeout = millis() + 10000;
                pirp->state = 10;
            }
            else if(millis() > pirp->timeout )
            {
                pirp->status = PIR_STATUS_INACTIVE;
                pirp->state = 10;
            } 
            break;
        case 30:
            if (millis() > pirp->timeout) {
                pirp->status = PIR_STATUS_INACTIVE;
                pirp->state = 10;
            }
            break;
        case 40:
            if (millis() > pirp->timeout) {
                pirp->state = 10;
            }
            break;
        case 50:
            if (millis() > pirp->timeout){
                pirp->state = 10;
            }
            break;

    }
    if (pirp->prev_state != pirp->state )
    {
        Serial.print(pirp->zone); Serial.print(F("/"));
        Serial.print(pirp->name); Serial.print(F(": "));
        Serial.print(pirp->prev_state); Serial.print(F("->:"));
        Serial.print(pirp->state); Serial.print(F(" status: "));
        Serial.print(pirp->status); Serial.print(F(": "));
        Serial.print(pirp->event); Serial.println();
        pirp->prev_state =pirp->state;
    }

    pirp->event = PIR_STATUS_UNDEFINED;
}


void pir_test_state_machine(pir_test_st *testp, pir_st *pirp)
{
    //Serial.print(pirp->state);
    //if(rfm_send_ready())
    {
        switch(pirp->state)
        {
            case 0:  // start
                // if(pirp->prev_active != pirp->event)
                // testp->timeout = millis() + testp->interval;
                // pirp->state = 10;
                break;
            case 10:  
                if( millis() > testp->timeout) 
                {
                    pirp->state = 20;
                    pirp->event = PIR_STATUS_ACTIVE;
                    //pir_send_radio_msg(pirp);
                }
                break;
            case 20:
                if( millis() > testp->timeout) 
                {
                    pirp->state = 10;
                    testp->timeout = millis() + testp->interval;
                    pirp->event = PIR_STATUS_INACTIVE;
                    //pir_send_radio_msg(pirp);
                }
                break;
        }
    }
}



void pir_task(void)
{
    switch(pir_handle.state)
    {
        case 0:
            pir_ctrl.sensor_indx = 0;
            pir_handle.state = 10;
            break;
        case 10:
            #ifdef TEST_MODE
            pir_test_state_machine(&pir_test[pir_ctrl.sensor_indx], &pir[pir_ctrl.sensor_indx]);
            #else
            pir_state_machine(&pir[pir_ctrl.sensor_indx]);
            #endif
            pir_ctrl.sensor_indx++;
            if (pir_ctrl.sensor_indx >= NBR_OF_PIR) pir_handle.state = 0;
            break;
    }

}

void pir_alarm_task(void)
{
    uint8_t pir_indx = 0;
    bool    tmp_alarm = false;
    switch(pir_alarm_handle.state)
    {
        case 0:
            pir_indx = 0;
            pir_alarm_handle.state = 10;
            break;
        case 10: // no alarms ?
            for(pir_indx = 0; pir_indx < NBR_OF_PIR; pir_indx++)
            { 
                if(pir[pir_indx].status == PIR_STATUS_ACTIVE )
                {
                    tmp_alarm = true;
                    Serial.printf("!!Alarm on @ %s-%s\n", pir[pir_indx].zone, pir[pir_indx].name);
                }
            }
            if (tmp_alarm) pir_alarm_handle.state = 20;
            break;
        case 20: // release alarms ?
            for(pir_indx = 0; pir_indx < NBR_OF_PIR; pir_indx++)
            { 
                if(pir[pir_indx].status == PIR_STATUS_ACTIVE )
                {
                    tmp_alarm = true;
                }
            }
            if (!tmp_alarm)
            {
                Serial.println("!!All alrms released");
                pir_alarm_handle.state = 10;  
            } 
            break;
    }

}


