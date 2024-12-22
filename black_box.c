#include <xc.h>
#include "main.h"

#define BLINK_ON 0x0F
#define BLINK_OFF 0x0C

extern unsigned char key;
unsigned short speed;

unsigned char *gear[9] = {"C_", "ON", "GN", "G1", "G2", "G3", "G4", "G5", "GR"};
unsigned char time[9];
unsigned char clock_reg[3];

unsigned char event[10];
extern char view_arr[10][15];
unsigned char dummy[11];
unsigned char ev_ind = 0, data_ind,data,add = 0;

unsigned char *menu[] = {"View Log         ", "Download Log    ", "Clear Log       ", "Set Time     "};

unsigned char log_event[10][16];
unsigned char index_arr[10] = {'1', '2', '3', '4'};
unsigned int delay = 0;
unsigned int hrs, min, sec,flag1 = 0,delay1 = 0,count = 0,event_count = 0,gr = 1,log_count = 0,ind = 0;


//function to get real time
void get_time(void) {
    clock_reg[0] = read_ds1307(HOUR_ADDR);
    clock_reg[1] = read_ds1307(MIN_ADDR);
    clock_reg[2] = read_ds1307(SEC_ADDR);

    if (clock_reg[0] & 0x40) {
        time[0] = '0' + ((clock_reg[0] >> 4) & 0x01);
        time[1] = '0' + (clock_reg[0] & 0x0F);
    } else {
        time[0] = '0' + ((clock_reg[0] >> 4) & 0x03);
        time[1] = '0' + (clock_reg[0] & 0x0F);
    }
    time[2] = ':';
    time[3] = '0' + ((clock_reg[1] >> 4) & 0x0F);
    time[4] = '0' + (clock_reg[1] & 0x0F);
    time[5] = ':';
    time[6] = '0' + ((clock_reg[2] >> 4) & 0x0F);
    time[7] = '0' + (clock_reg[2] & 0x0F);
    time[8] = '\0';
}

//Function to store events in the External EEPROM
void store_event() {

    //writing data to eeprom
    for (int i = 0; i < 8; i++) {
        write_EEPROM(ev_ind++, time[i]);
    }
    for (int i = 0; i < 2; i++) {
        write_EEPROM(ev_ind++, gear[gr][i]);
    }
    write_EEPROM(ev_ind++, (speed / 10) + '0');
    write_EEPROM(ev_ind++, (speed % 10) + '0');




    if (ev_ind >= 120) {
        ev_ind = 0;
    }

    event_count++;

}
//To Display the events in clcd
void view_dashboard() {


    get_time();

    speed = read_adc(CHANNEL4);

    speed = speed / 10.33;
    clcd_print("TIME     E  S", LINE1(1));

    clcd_print(time, LINE2(0));
    clcd_print(gear[gr], LINE2(10));
    clcd_putch(speed / 10 + '0', LINE2(13));
    clcd_putch(speed % 10 + '0', LINE2(14));

    if (key == MK_SW1) {
        gr = 0;
        store_event();
        //event_count++;

    }
    if ((key == MK_SW2) && (gr < 8)) {
        gr++;
        store_event();
        //event_count++;

    }
    if ((key == MK_SW3) && (gr > 1)) {
        gr--;
        store_event();
        //event_count++;
    }

    if (key == MK_SW11) {
        state = e_main_menu;
    }

}

// To display the main screen
void display_main_menu(void) {
    if (key == MK_SW11 && count < 3) {

        count++;

    }
    if (key == MK_SW12 && count > 0) {
        count--;
    }
    if (!count) {
        clcd_print("->", LINE1(0));
        clcd_print(menu[count], LINE1(2));
        clcd_print("  ", LINE2(0));
        clcd_print(menu[count + 1], LINE2(2));
    }
    else {
        clcd_print("  ", LINE1(0));
        clcd_print(menu[count - 1], LINE1(2));
        clcd_print("->", LINE2(0));
        clcd_print(menu[count ], LINE2(2));
    }
    if (count == 0 && key == MK_SW1) {
        state = e_view_log;
    }
    if (count == 1 && key == MK_SW1) {
        state = e_download_log;
    }
    if (count == 2 && key == MK_SW1) {
        state = e_clear_log;
    }
    if (count == 3 && key == MK_SW1) {
        state = e_set_time;
    }
    if (key == MK_SW2) {
        state = e_dashboard;
        CLEAR_DISP_SCREEN;
    }
}

//To view events stored in CLCD
void view_log(void) {

    clcd_print("L TIME     EV  SP   ", LINE1(0));
    event_reader();
    int end;
    
    if(event_count > 10)
    {
        end = 10;
    }
    else
    {
        end = event_count;
    }
    if (key == MK_SW11 && ind < end - 1) {
        ind++;
    }
    if (key == MK_SW12 && ind > 0) {
        ind--;
    }
    if(event_count == 0)
    {
        clcd_print("NO EVENTS STORED", LINE2(0));
    }
    else
    {
    clcd_putch((ind ) + '0', LINE2(0));
    clcd_print(log_event[ind], LINE2(2));
    }
    if (key == MK_SW2) {
        state = e_main_menu;
        CLEAR_DISP_SCREEN;
    }
}
//To read events stored in EEPROM
void event_reader(void) {
        int end;

    if (event_count > 10 )
    {
        add = ((event_count %10) * 12);
        end=10;
    }
    else
    {
    add = 0;
    
    end = event_count;
    }
    for (int i = 0; i < end; i++) {
        for (int j = 0; j < 15; j++) {
            if (j == 8 || j == 11) {
                log_event[i][j] = ' ';
            } else if (j == 14) {
                log_event[i][j] = '\0';
            } else {
                log_event[i][j] = read_EEPROM(add++);
                
                if(add >= 120 )
                    add = 0;
            }
        }
    }
}

//To display events stored in EEPROM in Tera Term
void download_log() {

    event_reader();
    clcd_print("DOWNLOAD                ", LINE1(0));
    clcd_print("   SUCCESSFUL           ", LINE2(0));
    int end;
    if(event_count > 9)
    {
        end = 10;
    }
    else
    {
        end = event_count;
    }
    if(event_count == 0)
    {
        puts("NO EVENTS PRESENT\n");
        state = e_main_menu;
    }
    else
    {
    while (delay++ == 100) {
        delay = 0;
        state = e_main_menu;
        CLEAR_DISP_SCREEN;
        puts("\n------------------");
        puts("\n\r");
        puts("L TIME     EV SP\n\r");
        
        puts("------------------");
        for (int i = 0; i < end; i++) {
            puts("\n\r");
            putch((i ) + '0');
            putch(' ');
            puts(log_event[i]);

        }
        puts("\n\r");
        puts("------------------\n\r");

    }
    }

}

//To clear the events Stored
void clear_log(void) {
    clcd_print("CLEARED                ", LINE1(0));
    clcd_print("SUCCESSFUL             ", LINE2(0));
    
    __delay_ms(1000);
        state = e_main_menu;
    event_count = 0;
}
//To print the time in clcd
void print_clcd(void) {
    clcd_putch(hrs / 10 + '0', LINE2(4));
    clcd_putch(hrs % 10 + '0', LINE2(5));

    clcd_putch(':', LINE2(6));

    clcd_putch(min / 10 + '0', LINE2(7));
    clcd_putch(min % 10 + '0', LINE2(8));

    clcd_putch(':', LINE2(9));

    clcd_putch(sec / 10 + '0', LINE2(10));
    clcd_putch(sec % 10 + '0', LINE2(11));

}
//To set the new  time 
void set_time(void) {
    clcd_print("    HH:MM:SS    ", LINE1(0));
    clcd_print("    ", LINE2(0));
    clcd_print("    ", LINE2(12));
    if (flag1 == 0) {
        hrs = ((time[0] - '0')*10) + (time[1] - '0');
        min = ((time[3] - '0')*10) + (time[4] - '0');
        sec = ((time[6] - '0')*10) + (time[7] - '0');
        flag1++;
    }
    if (key == MK_SW11) {
        if (flag1 == 1) { // Increment hours
            if (++hrs == 24) {
                hrs = 0;
            }
        } else if (flag1 == 2) { // Increment minutes
            if (++min == 60) {
                min = 0;
            }
        } else if (flag1 == 3) { // Increment seconds
            if (++sec == 60) {
                sec = 0;
            }
        }
    }

    if (flag1 == 1) {
        if (delay1++ <= 500) {
            print_clcd();
        } else if (delay1++ <= 1000) {
            clcd_putch(0xFF, LINE2(4));
            clcd_putch(0xFF, LINE2(5));

        } else {
            delay1 = 0;
        }
    }

    if (flag1 == 2) {
        if (delay1++ <= 500) {
            print_clcd();
        } else if (delay1++ <= 1000) {
            clcd_putch(0xFF, LINE2(7));
            clcd_putch(0xFF, LINE2(8));
        } else {
            delay1 = 0;
        }
    }

    if (flag1 == 3) {
        if (delay1++ <= 500) {
            print_clcd();
        } else if (delay1++ <= 1000) {
            clcd_putch(0xFF, LINE2(10));
            clcd_putch(0xFF, LINE2(11));
        } else {
            delay1 = 0;
        }
    }
    if (key == MK_SW12) {
        if (++flag1 == 4) {
            flag1 = 1;
        }
    }
    if (key == MK_SW1) {
        write_ds1307(HOUR_ADDR, (((hrs / 10) << 4) | (hrs % 10)));
        write_ds1307(MIN_ADDR, (((min / 10) << 4) | (min % 10)));
        write_ds1307(SEC_ADDR, (((sec / 10) << 4) | (sec % 10)));

        state = e_main_menu;
    }
    if (key == MK_SW2) {
        state = e_main_menu;
    }

}