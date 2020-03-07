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
#define I2C1_IRQSTATUS_RAW 0x24
#define I2C1_CM_PER 0x44E00048


#define conf_spi0_cs0 0x44E1095C
#define conf_spi0_d1  0x44E10958

/*Numbers Definess*/
#define I2C_CON_EN  (1 << 15)
#define I2C_START     0x1
#define I2C_STOP      0x2
#define DATA    0x5
#define SCLL 0x8
#define SCLH 0xA

/*Global Variables*/

volatile unsigned int USR_STACK[100];
volatile unsigned int INT_STACK[100];

/*Functions Prototypes*/
void initializeStack();
void configurePins();
void enableI2CClk();
void initializeI2C();
void setClkFreq();
void setClkSpeed();
void setOA();
void enableI2C();
void writeSA();
void writeNumberBytes(int numberBytes);
void pollBB();
void genStart();
void genStop();
void pollXRDY();
void writeToBuff();
void waitLoop(int numberOfLoops);

/*Main*/
int main() {
    //initializeStack();

    configurePins();

    enableI2CClk();

    setClkFreq();

    initializeI2C();

    writeSA();

    while (1){
        pollBB();

        writeNumberBytes(1);

        genStart();

        pollXRDY();

        writeToBuff();

   }
    waitLoop(500);



return 0;
}

/*Functions Definitions*/

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
    HWREG(I2C1_CM_PER) |= 0x2;
}

void setClkFreq(){
    HWREG(I2C1_BASE + I2C1_PSC) |= 0x3;
}

void initializeI2C(){
    setClkSpeed();
    // setOA();
    enableI2C();
}

void setClkSpeed(){
    HWREG(I2C1_BASE + I2C1_SCLL) |= SCLL;
    HWREG(I2C1_BASE + I2C1_SCLH) |= SCLH;
}

void setOA(){
    HWREG(I2C1_BASE + I2C1_OA) = 0x0;
}

void enableI2C(){
    HWREG(I2C1_BASE + I2C1_CON) |= 0x00008600;
}

void writeSA(){
    HWREG(I2C1_BASE + I2C1_SA) |= 0x60;
}

void writeNumberBytes(int numberBytes){
    HWREG(I2C1_BASE + I2C1_CNT) |= numberBytes;
}

void pollBB(){
    
    while (HWREG(I2C1_BASE + I2C1_IRQSTATUS_RAW) & (1<<12) == 1){
        //do nothing
    }

}

void genStart(){
    HWREG(I2C1_BASE + I2C1_CON) |= I2C_START;
}

void genStop(){
    HWREG(I2C1_BASE + I2C1_CON) |= I2C_STOP;
}

void pollXRDY(){

    while ( HWREG(I2C1_BASE + I2C1_IRQSTATUS_RAW) & (1<<4) == 0 ){
        //do nothing
    }

}

void writeToBuff(){
    HWREG(I2C1_BASE + I2C1_DATA) = DATA;
}

void waitLoop(int numberOfLoops){
    
    while(numberOfLoops > 0){
        numberOfLoops--;
    }
}