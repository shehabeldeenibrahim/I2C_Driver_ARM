Initializations:
Set clock freq to 12Mhz:
    write ((48MHz/ 12 MHz) -1) into I2C_PSC at 0x4802A0B0 bits[7:0]
Set SCL to 400 kbps:
    write (1/400,000)/(1/12x10^6) - 7 SCLL at 0x4802A0B4
    write (1/400,000)/1/12x10^6 - 5 SCLH at 0x4802A0B8
Set I2C own address to 7 bits:
    write 0x0000 into I2C_OA at 0x4802A0A8
Take I2C out of reset:
    write 0x1 into I2C_CON at 0x4802A0A4 enable bit [15]

Writing to I2C:
Write motor Slave address:
    write 0x60 to I2C_SA[9:0] at 0x4802A0AC
    write numberOfBytes to I2C_CNT[15:0] 0x4802A098
Loop till BB = 0:
    Check if I2C_IRQSTATUS_RAW[12] = 0 at 0x4802A024
    no -> keep looping
    yes ->
    Generate start condition:
        Write 0x00008601 into I2C_CON at 0x4802A0A4
    Check if I2C_IRQSTATUS_RAW XRDY bit [4] = 1 at 0x4802A024:
    no -> keep looping
    yes -> 
        write DATA into I2C_DATA at 0x4802A09C bits[7:0]
Generate Stop Condition:
    Write 0x00008602 into I2C_CON at 0x4802A0A4