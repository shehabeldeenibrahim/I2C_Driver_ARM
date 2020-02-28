Adjust clk freq on 12 MHz
Adjust SCL to 400 Kb/s for F/S mode
Set I2C own address to 7 bits
Take I2C out of reset
Write motor Slave address
Generate Start condition
Keep Checking if BB is 0
if BB is 0 ->
Generate Start condition
Write a byte of data into buffer
Generate Stop condition