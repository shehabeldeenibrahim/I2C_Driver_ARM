#include <stdio.h>
#include <stdbool.h>

#define HWREG(x) (* ((volatile unsigned int *)(x)))

/*Addresses Defines*/
#define I2C1_BASE   0x4802A000
#define I2C1_SCLK   0xB0
#define I2C1_SCLL   0xB4
#define I2C1_SCLH   0xB8
#define I2C1_OA     0xA8
#define I2C1_CON    0xA4
#define I2C1_SA     0xAC
#define I2C1_CNT    0x98
#define I2C1_DATA   0x9C
#define I2C1_IRQSTATUS_RAW 0x24



/*Numbers Definess*/
#define I2C_CON_EN  (1 << 15)
#define DATA    0x5
#define tLow 23
#define tHigh 25

/*Global Variables*/
int BB;
int XRDY;

/*Functions Prototypes*/
void initializeStack();
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

    initializeI2C();
    writeSA();
    writeNumberBytes();
    pollBB();
    genStart();
    /*pollXRDY();*/
    writeToBuff();
    genStop();


return 0;
}

/*Functions Definitions*/
void initializeStack(){
    
}
void setClkFreq(){

}
void initializeI2C(){
    setClkSpeed();
    setOA();
    enableI2C();
}

void setClkSpeed(){
    HWREG(I2C1_BASE + I2C1_SCLL) = tLow;
    HWREG(I2C1_BASE + I2C1_SCLH) = tHigh;
}
void setOA(){
    HWREG(I2C1_BASE + I2C1_OA) = 0x0000;
}
void enableI2C(){
    HWREG(I2C1_BASE + I2C1_CON) = I2C_CON_EN;
}
void writeSA(){
    HWREG(I2C1_BASE + I2C1_SA) = 0x60;
}
void writeNumberBytes(){
    HWREG(I2C1_BASE + I2C1_CNT) = 0x1;
}
bool pollBB(){
    do{

        BB = HWREG(I2C1_BASE + I2C1_IRQSTATUS_RAW);
        BB = (BB & (1<<12));

    } while (BB = 0);
    return true;
}
void genStart(){
    HWREG(I2C1_BASE + I2C1_CON) =0x00008601;
}
void genStop(){
    HWREG(I2C1_BASE + I2C1_CON) =0x00008602;
}
bool pollXRDY(){
    do{

        XRDY = HWREG(I2C1_BASE + I2C1_IRQSTATUS_RAW);
        XRDY = (XRDY & (1<<4));

    } while (XRDY = 0);
    return true;
}
void writeToBuff(){
    HWREG(I2C1_BASE + I2C1_DATA) = DATA;
}