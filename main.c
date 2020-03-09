#include <stdio.h>
#include <stdbool.h>

#define HWREG(x) (* ((volatile unsigned int *)(x)))

/*Addresses Defines*/
#define I2C1_BASE   0x4802A000
#define I2C1_PSC    0xB0
#define I2C1_SCLL   0xB4
#define I2C1_SCLH   0xB8
#define I2C1_OA     0xA8
#define I2C1_CON    0xA4
#define I2C1_SA     0xAC
#define I2C1_CNT    0x98
#define I2C1_DATA   0x9C
#define I2C1_CM_PER 0x44E00048
#define I2C1_IRQSTATUS_RAW 0x24


#define conf_spi0_cs0 0x44E1095C
#define conf_spi0_d1  0x44E10958

/*Numbers Definess*/
#define I2C_CON_EN  (1 << 15)
#define SCLL 0x8
#define SCLH 0xA

/*Global Variables*/
int BB;
int XRDY;
int numberBytes = 0x2;
int i = 20;
int temp;
volatile unsigned int USR_STACK[100];
volatile unsigned int INT_STACK[100];

/*PWMs*/
int PWM2_ON_H  = 0x0F;
int PWM3_ON_H  = 0x13;
int PWM4_ON_H  = 0x17;
int PWM5_ON_H  = 0x1B;
int PWM6_ON_H  = 0x1F;
int PWM7_ON_H  = 0x23;
int PWM_H_VALUE = 0x10;
int PWM_L_VALUE = 0x00;


/*Struct of data and address*/
struct message
{
    int address;
    int data;
};

/*Functions Prototypes*/
void sendCommands(int dCount, struct message COMMANDS[]);
void sendPWM(int pwm_dCount, struct message PWM[]);
void initializeStack();
void configurePins();
void enableI2CClk();
void initializeI2C();
void setClkFreq();
void setClkSpeed();
void setOA();
void enableI2C();
void writeSA(int address);
void writeNumberBytes();
bool pollBB();
void genStart();
void genStop();
bool pollXRDY();
void writeToBuff();
void waitLoop(int loops);


/*Main*/
int main() {

    int dCount = 5;
    struct message COMMANDS[5] = {
        {0x0, 0x11},    //sleep, allcall
        {0xFE, 5},      //prescale
        {0x0, 0x81},    //restart, allcall, interior clk
        {0x01, 0x4},    //totem, invert
        {0xFD, 0x0}     //ALL_LED_OFF_H
    };

    int pwm_dCount = 24;
    struct message PWM[24] = {
            {PWM4_ON_H, PWM_H_VALUE},    //PWM4
            {PWM3_ON_H, PWM_L_VALUE},    //PWM3
            {PWM5_ON_H, PWM_H_VALUE},    //PWM5
            {PWM6_ON_H, PWM_L_VALUE},    //PWM6
            {PWM2_ON_H, PWM_H_VALUE},    //PWM2
            {PWM7_ON_H, PWM_H_VALUE},    //PWM7

            {PWM4_ON_H, PWM_H_VALUE},    //PWM4
            {PWM3_ON_H, PWM_L_VALUE},    //PWM3
            {PWM5_ON_H, PWM_L_VALUE},    //PWM5
            {PWM6_ON_H, PWM_H_VALUE},    //PWM6
            {PWM2_ON_H, PWM_H_VALUE},    //PWM2
            {PWM7_ON_H, PWM_H_VALUE},    //PWM7

            {PWM4_ON_H, PWM_L_VALUE},    //PWM4
            {PWM3_ON_H, PWM_H_VALUE},    //PWM3
            {PWM5_ON_H, PWM_L_VALUE},    //PWM5
            {PWM6_ON_H, PWM_H_VALUE},    //PWM6
            {PWM2_ON_H, PWM_H_VALUE},    //PWM2
            {PWM7_ON_H, PWM_H_VALUE},    //PWM7

            {PWM4_ON_H, PWM_L_VALUE},    //PWM4
            {PWM3_ON_H, PWM_H_VALUE},    //PWM3
            {PWM5_ON_H, PWM_H_VALUE},    //PWM5
            {PWM6_ON_H, PWM_L_VALUE},    //PWM6
            {PWM2_ON_H, PWM_H_VALUE},    //PWM2
            {PWM7_ON_H, PWM_H_VALUE},    //PWM7
        };


    configurePins();

    enableI2CClk();

    HWREG(I2C1_CM_PER) = 0x2;

    setClkFreq();

    initializeI2C();

    writeNumberBytes();
    writeSA(0x60);

    sendCommands(dCount, COMMANDS);

    sendPWM(pwm_dCount, PWM);
    pollBB(); //debug

return 0;
}

/*Functions Definitions*/

void sendCommands(int dCount, struct message COMMANDS[]){
    for(int i = 0; i < dCount; i++){

        pollBB();
        writeNumberBytes();
        genStart();
        pollXRDY();
        //write to buffer
        HWREG(I2C1_BASE + I2C1_DATA) = COMMANDS[i].address;
        HWREG(I2C1_BASE + I2C1_DATA) = COMMANDS[i].data;
        for(int k = 0; k < 5000; k++){}

    }
}
void sendPWM(int pwm_dCount, struct message PWM[]){
    for(int j = 0; j < 200; j++){
    for(int i = 0; i < pwm_dCount; i++){
            pollBB();
            writeNumberBytes();
            genStart();
            pollXRDY();
            if(i % 5 == 0){
                for(int k = 0; k < 400000; k++){}
            }
            //write to buffer
            HWREG(I2C1_BASE + I2C1_DATA) = PWM[i].address;
            HWREG(I2C1_BASE + I2C1_DATA) = PWM[i].data;


        }

    }
}

void initializeStack(){
    //SET UP STACKS
    //init USR stack
    asm("LDR R13, =USR_STACK");
    asm("ADD R13, R13, #0x100");

}

void configurePins(){
    HWREG(conf_spi0_cs0) = 0x2A;
    HWREG(conf_spi0_d1) = 0x2A;
}

void enableI2CClk(){
    HWREG(I2C1_CM_PER) = 0x2;
}

void setClkFreq(){
    HWREG(I2C1_BASE + I2C1_PSC) = 3;
}

void initializeI2C(){
    setClkSpeed();
    // setOA();
    enableI2C();
}

void setClkSpeed(){
    HWREG(I2C1_BASE + I2C1_SCLL) = SCLL;
    HWREG(I2C1_BASE + I2C1_SCLH) = SCLH;
}

void setOA(){
    HWREG(I2C1_BASE + I2C1_OA) = 0x0;
}

void enableI2C(){
    HWREG(I2C1_BASE + I2C1_CON) = 0x00008600;
}

void writeSA(int address){
    HWREG(I2C1_BASE + I2C1_SA) = address;
}

void writeNumberBytes(){
    HWREG(I2C1_BASE + I2C1_CNT) = numberBytes;
}

bool pollBB(){
    do{

        BB = HWREG(I2C1_BASE + I2C1_IRQSTATUS_RAW);
        BB = ((BB >> 12) & 1);

    } while (BB == 1);
    return true;
}

void genStart(){
    HWREG(I2C1_BASE + I2C1_CON) =0x00008603;
}

void genStop(){
    HWREG(I2C1_BASE + I2C1_CON) =0x00008602;
}

bool pollXRDY(){
    do{

        XRDY = HWREG(I2C1_BASE + I2C1_IRQSTATUS_RAW);
        XRDY = ((XRDY >> 4) & 1);

    } while (XRDY == 0);
    return true;
}

void writeToBuff(){
    //HWREG(I2C1_BASE + I2C1_DATA) = DATA;
}
void waitLoop(int loops){
    for (int i = 0; i < loops; i++){
        //wait
    }
}
