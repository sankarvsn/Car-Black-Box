/*
 * File:   main.c
 * Author: V.Sankar Narayana
 *
 * Created on 06 December, 2024,
 */
#include<xc.h>
#include "main.h"

State_t state;

void init_config() {
    init_matrix_keypad();
    init_clcd();
    init_adc();
    init_i2c();
    init_ds1307();
    init_uart();
    state = e_dashboard;
}
unsigned char key;
unsigned short speed;
unsigned int menu = 0;

void main(void) {
    init_config();
    //store_event();
    while (1) {
        // Detect key press
        key = read_switches(STATE_CHANGE);

        switch (state) {
            case e_dashboard:
                // Display dashboard
                view_dashboard();
                break;

            case e_main_menu:
                // Display main menu
                display_main_menu();
                break;

            case e_view_log:
                // Display view log (stored events in clcd)
                view_log();
                break;

            case e_download_log:
                //Display Stored events in Tera term using UART
                download_log();
                break;

            case e_clear_log:
                //Clear the events stored
                clear_log();
                break;

            case e_set_time:
                //Setting new time
                set_time();
                break;

        }

    }
}
