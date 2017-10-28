Attiny85 read Frequency with pin change interrupt

Basically, this project reads frequency by pin change interrupt and sends the data by IC2 to master.

To be more specific, this program uses slave library to connect with I2C and uses PCINT1 at PB1 to collect falling edge data of the input frequency for a specified time. External counter interrupt  mostly set for frequency reading; however, Attiny85 has one external timer interrupt, which located at pin 2 and overlaps with SCK pin of the I2C.   

You can use the Makefile without changing. masterFreq.cpp file is not for the attiny. Raspberry pi used as a master device in this project. 

Slave libraries are used from the "https://github.com/eriksl/usitwislave". If you want the original ones you can access from here.
