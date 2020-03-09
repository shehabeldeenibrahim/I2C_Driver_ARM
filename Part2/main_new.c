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
//#define DATA    0x5
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

/*Functions Prototypes*/
void data(int dCount, int DATA[]);
void initializeStack();
void configurePins();
void enableI2CClk();
void initializeI2C();
void setClkFreq();
void setClkSpeed();
void setOA();
void enableI2C();
void writeSA();
void writeNumberBytes();
bool pollBB();
void genStart();
void genStop();
bool pollXRDY();
void writeToBuff();


/*Main*/
int main() {

    int DATA[10] = {
                0x00, 0x11, //sleep, allcall
                0xFE, 0x05,   //prescale
                0x00, 0x81, //restart, allcall, interior clk
                0x01, 0x04, //totem, invert
                0xFD, 0x00  //ALL_LED_OFF_H 
                    };
    int dCount = 7;

    configurePins();

    enableI2CClk();

    HWREG(I2C1_CM_PER) = 0x2;

    setClkFreq();

    initializeI2C();

    writeNumberBytes();

    writeSA();

    data(dCount, DATA);

return 0;
}

/*Functions Definitions*/

void data(int dCount, int DATA[]){
    for(int i = 0; i < dCount; i++){
        pollBB();
        writeNumberBytes();
        genStart();
        pollXRDY();
        //write to buffer
        HWREG(I2C1_BASE + I2C1_DATA) = DATA[i];
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

void writeSA(){
    HWREG(I2C1_BASE + I2C1_SA) = 0x60;
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
