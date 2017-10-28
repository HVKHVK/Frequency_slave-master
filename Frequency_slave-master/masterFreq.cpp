#include <iostream>
#include <errno.h>
#include <unistd.h> // sleep()
#include <stdint.h> // uint8_t, uint16_t
#include <wiringPiI2C.h>

using namespace std;

int main()
{
	int sent, data, low, high, id;
    uint8_t lowbyte;
    uint16_t total;
    
	sent = wiringPiI2CSetup (0x03); // the device address goes here

	while (1) // turn ones
	{
		wiringPiI2CWrite (sent, 1); // we send the command 1 to the ATtiny

		// the output buffer is empty after a write, get the 0 and ignore
		data = wiringPiI2CRead (sent);

		sleep(1);

		high = wiringPiI2CRead (sent);
		//cout << "High: "<< high << endl;
        
        low = wiringPiI2CRead (sent);
			
        sleep(1);
		id = wiringPiI2CRead(sent);

        lowbyte = (uint8_t)low;
        total = high << 8 | lowbyte;

		cout << "Freq: "<< total << endl;
		cout << "ID: "<< id << endl;
        cout << "\n" << endl;
        break;
	}	
}

