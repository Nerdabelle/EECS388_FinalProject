#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "eecs388_lib.h"
#include "metal/i2c.h"

struct metal_i2c *i2c;
uint8_t bufWrite[5];
uint8_t bufRead[1];

//The entire setup sequence
void set_up_I2C()
{
    uint8_t oldMode;
    uint8_t newMode;
    _Bool success;

    bufWrite[0] = PCA9685_MODE1;
    bufWrite[1] = MODE1_RESTART;
    printf("%d\n",bufWrite[0]);
    
    i2c = metal_i2c_get_device(0);

    if(i2c == NULL){
        printf("Connection Unsuccessful\n");
    }
    else{
        printf("Connection Successful\n");
    }
    
    //Setup Sequence
    metal_i2c_init(i2c,I2C_BAUDRATE,METAL_I2C_MASTER);
    success = metal_i2c_write(i2c,PCA9685_I2C_ADDRESS,2,bufWrite,METAL_I2C_STOP_DISABLE);//reset
    delay(100);
    printf("resetting PCA9685 control 1\n");

    //Initial Read of control 1
    bufWrite[0] = PCA9685_MODE1;//Address
    success = metal_i2c_transfer(i2c,PCA9685_I2C_ADDRESS,bufWrite,1,bufRead,1);//initial read
    printf("Read success: %d and control value is: %d\n", success, bufWrite[0]);
    
    //Configuring Control 1
    oldMode = bufRead[0];
    newMode = (oldMode & ~MODE1_RESTART) | MODE1_SLEEP;
    printf("sleep setting is %d\n", newMode);
    bufWrite[0] = PCA9685_MODE1;//address
    bufWrite[1] = newMode;//writing to register
    success = metal_i2c_write(i2c,PCA9685_I2C_ADDRESS,2,bufWrite,METAL_I2C_STOP_DISABLE);//sleep
    bufWrite[0] = PCA9685_PRESCALE;//Setting PWM prescale
    bufWrite[1] = 0x79;
    success = metal_i2c_write(i2c,PCA9685_I2C_ADDRESS,2,bufWrite,METAL_I2C_STOP_DISABLE);//sets prescale
    bufWrite[0] = PCA9685_MODE1;
    bufWrite[1] = 0x01 | MODE1_AI | MODE1_RESTART;
    printf("on setting is %d\n", bufWrite[1]);
    success = metal_i2c_write(i2c,PCA9685_I2C_ADDRESS,2,bufWrite,METAL_I2C_STOP_DISABLE);//awake
    delay(100);
    printf("Setting the control register\n");
    bufWrite[0] = PCA9685_MODE1;
    success = metal_i2c_transfer(i2c,PCA9685_I2C_ADDRESS,bufWrite,1,bufRead,1);//initial read
    printf("Set register is %d\n",bufRead[0]);
} 

void breakup(int bigNum, uint8_t* low, uint8_t* high)
{
// your code from Milestone 1
    *low = bigNum & 0xFF; //0xFF masks the higher bits
    *high = (bigNum >> 8);
}

void steering(int angle)
{
// your code from Milestone 1
    int cycles = getServoCycle(angle);
    uint8_t l, h;
    breakup(cycles, &l, &h);
    
    bufWrite[0] = PCA9685_LED0_ON_L + 0x04; //servo address
    bufWrite[1] = 0x00;//need to be on for metal_i2c_transfer
    bufWrite[2] = 0x00;//need to be on for metal_i2c_transfer
    bufWrite[3] = l;
    bufWrite[4] = h;

    metal_i2c_transfer(i2c, PCA9685_I2C_ADDRESS, bufWrite, 5, bufRead, 1);
}

void stopMotor()
{
// your code from Milestone 1
    uint8_t l, h;
    breakup(280, &l, &h);
    
    bufWrite[0] = PCA9685_LED0_ON_L;//servo address
    bufWrite[1] = 0x00;//need to be on for metal_i2c_transfer
    bufWrite[2] = 0x00;//need to be on for metal_i2c_transfer
    bufWrite[3] = l;
    bufWrite[4] = h;
    
    metal_i2c_transfer(i2c, PCA9685_I2C_ADDRESS, bufWrite, 5, bufRead, 1);
}

void driveForward(uint8_t speedFlag)
{
// your code from Milestone 1
    uint8_t l, h;
    switch(speedFlag) {
        case 1:
            breakup(313, &l, &h);
        break;
        case 2:
            breakup(315, &l, &h);
        break;
        case 3:
            breakup(317, &l, &h);
        break;
        default:
            return;
        break;
    }

    bufWrite[0] = PCA9685_LED0_ON_L;
    bufWrite[1] = 0x00;//need to be on for metal_i2c_transfer
    bufWrite[2] = 0x00;//need to be on for metal_i2c_transfer
    bufWrite[3] = l;
    bufWrite[4] = h;
    
    metal_i2c_transfer(i2c, PCA9685_I2C_ADDRESS, bufWrite, 5, bufRead, 1);
}

void driveReverse(uint8_t speedFlag)
{
// your code from Milestone 1
    uint8_t l, h;
    switch(speedFlag) {
        case 1:
            breakup(267, &l, &h);
        break;
        case 2:
            breakup(265, &l, &h);
        break;
        case 3:
            breakup(263, &l, &h);
        break;
        default:
            return;
        break;
    }
    bufWrite[0] = PCA9685_LED0_ON_L;
    bufWrite[1] = 0x00;//need to be on for metal_i2c_transfer
    bufWrite[2] = 0x00;//need to be on for metal_i2c_transfer
    bufWrite[3] = l;
    bufWrite[4] = h;
    
    metal_i2c_transfer(i2c, PCA9685_I2C_ADDRESS, bufWrite, 5, bufRead, 1);
}

void raspberrypi_int_handler(int devid, int * angle, int * speed, int * duration)
{
    char * str = malloc(20 * sizeof(char)); // you can use this to store the received string
                                            // it is the same as char str[20]                

   // Extract the values of angle, speed and duration inside this function
   // And place them into the correct variables that are passed in
    //int i = 0;
    /*
    while(1){
        str[i]= ser_read(1);
        if(str[i-2]=='d' && str[i-1]==':'){
            break;
        }
        i++;
    }
    */
    ser_readline(devid, 20, str);
    sscanf(str, "a:%d s:%d d:%d", angle, speed, duration);

    free(str);
}


int main()
{
    // Initialize I2C
    set_up_I2C();
    delay(2000);

    // Calibrate Motor
    printf("Calibrate Motor.\n");
    stopMotor();
    delay(2000);

    // initialize UART channels
    ser_setup(0); // uart0 (receive from raspberry pi)
    ser_setup(1);  
    printf("Setup completed.\n");
    printf("Begin the main loop.\n");
    

    int angle, speed, duration;
    // Drive loop
    while (1) {
        // The following pseudo-code is a rough guide on how to write your code
        // It is NOT actual C code
         /*  
          if (is UART0 ready?)
          {
              call raspberrypi_int_handler() to get values of angle, speed and duration

              call steering(), driveForward/Reverse() and delay with the extracted values
          }
        */

        if(ser_isready(1))
        {
            raspberrypi_int_handler(1, &angle, &speed, &duration);
            printf("Angle:%d Speed:%d Duration:%d\n", angle, speed, duration);
            
            steering(angle);
            if(speed>0){
                driveForward(speed);
            }
            else if(speed<0){
                driveReverse(speed*-1);
            }
            else{
                stopMotor();
            }
            delay(duration*1000);
        }
    }
    return 0;
}
