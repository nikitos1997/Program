// Developer note stack size needs to be increased to allow the allocation of memory for this double = 8 bytes
// 8 * 24 * 60 * 60 * 2 arrays of doubles = 1382400 bytes base allocation is 1MB 
// In VS2017 go to project > properties > System > and set Stack Reserve Size and Stack Commit Size to 2000000
// This prevents a stack overflow

#define maximum_readings 24 * 60 * 60
#define invalid_reading  -1000.0


#define fakedata 1	 // NOTE:  Set to 1 use fake sensor values instead of the online values 0

#include <chrono>  // System time
#include <stdexcept>
#include <vector>
#include <iostream>

#if fakedata == 1

#include "fakesensor.hpp"

#else

#include "http.h"

#endif


using namespace std;
using namespace chrono;


class Climate {

private:
	//define stuct constructor and use structure in vector
	struct READINGS
	{
		int seconds;
		double temperature;
		double humidity;
	};

	Climate::READINGS reading;

	std::vector<Climate::READINGS> samples;

	system_clock::time_point startTime; //Start time of when object is made

	bool Climate::ResizeVector(int); //Resizing the vector 

	//Mutators
	void SetSeconds(int addressPointer, double value) { samples[addressPointer].seconds = value; };
	void SetTemperature(int addressPointer, double value) { samples[addressPointer].temperature = value; };
	void SetHumidity(int addressPointer, double value) { samples[addressPointer].humidity = value; };

#if fakedata == 1	
	FakeSensor sensorDevice;
#else
	Http sensorDevice;
#endif

protected:

public:

	// Constructors
	Climate();

	// Utility
	void clearSamples();
	long sampleCount(long);
	long sampleTotal();
	double getTemperature(long);
	double getHumidity(long);
	double averageHumidity(long);
	double averageTemperature(long);

	// Sensor related
	long	readSensor();

	//Accessors
	double Seconds(int addressPointer) { return samples[addressPointer].seconds; };
	double Temperature(int addressPointer){ return samples[addressPointer].temperature; };
	double Humidity(int addressPointer){ return samples[addressPointer].humidity; };
};

// Constructor
Climate::Climate() {

	startTime = std::chrono::system_clock::now();

	if (samples.size() == 0)
	{
		samples.resize(1); // Sets to 1 when new object is made (so that it can at least hold 1 reading)
	}
}
//Clears all the samples taken of the samples vector
void	Climate::clearSamples() 
{
	for (int i = 0; i < samples.size(); i++)//Loop to go through the samples and change them all to 0
	{
		SetSeconds(i, 0);
		SetTemperature(i, 0);
		SetHumidity(i, 0);
	}
	samples.resize(1);//Resize the vector to 1
}
//Reading the sensor and storing the data in the vector 
long	Climate::readSensor() {

	bool readingTaken = false;
	int lastSampleSec = 0;

	try
	{
		sensorDevice.read_data();//Reading is taken
		readingTaken = true;
	}
	catch (const runtime_error& error) //Catching run time error
	{
		cout << "Read attempt fail" << endl;
		readingTaken = false;
	}
	system_clock::time_point endTime = std::chrono::system_clock::now();//Getting end time of the reading

	std::chrono::duration<double> timeTook = endTime - startTime; //Calculating duration of how long it took to read

	long currentSecond = timeTook.count();//Capturing current second in the variable
	lastSampleSec = currentSecond - 1;//Last sample is current second -1

	if (readingTaken == true)
	{
		try
		{
			reading.seconds = currentSecond;
			reading.temperature = sensorDevice.get_temperature_in_c();
			reading.humidity = sensorDevice.get_humidity();
			int address = currentSecond;

			bool needsResize = ResizeVector(address); //Resizing samples vector if needed to be resized (resize is done inside method)

			if (currentSecond >= maximum_readings) //If current second has reached the maximum readings, exception is thrown, cannot be stored
			{
				throw std::out_of_range(""); //throw out of range
				needsResize = false;
			}
			if (currentSecond != lastSampleSec + 1)//If current second is not from the last sample second + 1
			{
				throw std::underflow_error("");//throw underflow error
				needsResize = false;
			}
			if (needsResize == true)//If resize was needed, new element can be added 
			{
				//samples[address] = reading; //reading is taken
				SetSeconds(address, reading.seconds);
				SetTemperature(address, reading.temperature);
				SetHumidity(address, reading.humidity);
			}
			if (needsResize == false)//If reading was not resized
			{
				cout << "Resize error: reading was not recorded" << endl;
			}
		}
		catch (const out_of_range& error)//catching out of range error
		{
			cout << "The reading cannot be stored because it is out of range of vector" << endl;
		}
		catch (const underflow_error& error)//catching underflow error
		{
			cout << "The sample is less than 1 second from previous" << endl;
		}
	}

	// This line is purely for your debugging and can be removes/commented out in the final code.
	cout << endl << "Debugging information : "  << "Temperature is " << sensorDevice.get_temperature_in_c() << " in degrees C " << sensorDevice.get_humidity() << "% humidity"<< endl;

	return currentSecond;
}
//Counts the number of samples that were taken by looking back over number of seconds
long Climate::sampleCount(long secs) 
{
	int sampleCount = 0;
	int addressPointer = 0;
	bool error = false;

	system_clock::time_point currentTime = std::chrono::system_clock::now();//Getting current time

	std::chrono::duration<double> timeTook = currentTime - startTime; //Getting current second from start time and time now

	try
	{
		addressPointer = (int)timeTook.count() - secs; // pointer to address minus the seconds from current time

		if (addressPointer <= 0 || addressPointer >= maximum_readings)// address pointer cannot be zero or less and more than max readings
		{
			error = true;
			throw std::out_of_range(""); //throwing out of range exception
			sampleCount = 0;
		}
		if (secs > samples.size()) //secs to look back over cannot be higher than the size of the vector
		{
			error = true;
			throw std::out_of_range(""); // throwing out of range exception
			sampleCount = 0;
		}
		if (addressPointer > 0 && error == false)
		{
			for (int pointer = addressPointer; pointer < (int)timeTook.count(); pointer++) // Looping through to add one to sample count until current second is reached
			{
				sampleCount++;
			}
		}
	}
	catch (const out_of_range& error) //handling out of range with message
	{
		cout << "\nThe number of readings taken is less than the specified seconds to look back over or it is outside of 0-24 hour limit" << endl;
	}
	return sampleCount;
}
//Resizing the vector method to resize the vector when each sample is taken and for each hour
bool Climate::ResizeVector(int address)
{
	bool resized = false;
	int hours[24] = {}; //array of hours

	hours[0] = 3600; //first hour is defined
	for (int i = 0; i < 24; i++) //loop through to generate hours
	{
		hours[i] = hours[i-1] + 3600; //each possible hour in 24 hour period is generated in seconds

		if (address >= hours[i]) //Checks if address parameter falls within that hour
		{
			samples.resize(hours[i] + 3600); //resizes the samples vector
			resized = true; //returns true
			break;
		}
	}
	if (resized == false && address >= samples.size()) //Checks if vector hasnt been resized and resizes it for new value
	{
		samples.resize(address + 1);
		samples[address] = reading;
		resized = true;
	}
	return resized;
}
//Calculates the total number of samples that were taken.
long Climate::sampleTotal()
{
	long total = 0;

	for (int counter = 0; counter < samples.size(); counter++)
	{
		if (samples[counter].seconds != 0)
		{
			total++;
		}
	}
	return total;
}
//Gets the temperature based on the seconds of the reading
double Climate::getTemperature(long secs)
{
	double caughtTemperature = 0; //Variable to store the temperature in

	try
	{
		if (secs > samples.size() || secs <= 0)//If seconds provided are out of range of vector or less than 0
		{
			throw std::out_of_range("");//throw out of range error
		}
		if (samples[secs].temperature == 0)//if temperature of that second is 0
		{
			throw std::invalid_argument("");//throw invalid argument
		}
		if (samples[secs].temperature > 0 && secs <= samples.size())//If temperature is more than 0 and secs are within range of vector
		{
			caughtTemperature = Temperature(secs); //Using accessor to get temperature of the reading by passing second as an index
		}
	}
	catch (const out_of_range& error)//Handling the out of range
	{
		cout << "Outside of the 0-24 hour limit" << endl;
	}
	catch (const invalid_argument& error)//Handling invalid argument
	{
		cout << "No sample at the requested time" << endl;
	}
	return caughtTemperature;
}
//Method to get humidity of the sample based on the provided second
double Climate::getHumidity(long secs)
{
	double caughtHumidity = 0; //Variable to store the temperature in

	try
	{
		if (secs > samples.size() || secs <= 0)//If seconds provided are out of range of vector or less than 0
		{
			throw std::out_of_range("");//throw out of range error
		}
		if (samples[secs].humidity == 0)//if humidity of that second is 0
		{
			throw std::invalid_argument("");//throw invalid argument
		}
		if (samples[secs].humidity > 0 && secs <= samples.size())//If humidity is more than 0 and secs are within range of vector
		{
			caughtHumidity = Humidity(secs); //Accesing humidity using seconds as index for the vector
		}
	}
	catch (const out_of_range& error)//Handling the out of range
	{
		cout << "Outside of the 0-24 hour limit" << endl;
	}
	catch (const invalid_argument& error)//Handling invalid argument
	{
		cout << "No sample at the requested time" << endl;
	}
	return caughtHumidity;
}
//Method that calculates the average humidity looking back over the seconds from current time
double Climate::averageHumidity(long secs)
{
	double averageHumidity = 0; //Variable to store the temperature in
	int counter = 0;

	system_clock::time_point currentTime = std::chrono::system_clock::now();//Getting current time

	std::chrono::duration<double> timeTook = currentTime - startTime; //Getting current second from start time and time now

	try
	{
		int addressPointer = (int)timeTook.count() - secs;

		if (addressPointer > samples.size() || addressPointer <= 0 || addressPointer>=maximum_readings)//If address pointer is more than the size of vector or less than 0
		{
			throw std::out_of_range("");//throw out of range error
		}
		if (samples[addressPointer].humidity == 0)//if humidity of the address pointer is 0, then humidity has not been taken
		{
			throw std::invalid_argument("");//throw invalid argument
		}
		if (samples[addressPointer].humidity > 0 && secs <= samples.size())//If humidity is more than 0 and secs are within range of vector
		{
			for (addressPointer; addressPointer < (int)timeTook.count(); addressPointer++)//Starting from address pointer we loop till the current second
			{
				if (samples[addressPointer].humidity == 0)//if humidity of the address pointer is 0, then humidity has not been taken
				{
					throw std::invalid_argument("");//throw invalid argument
				}
				averageHumidity += Humidity(addressPointer);//Adds the humidity of the reading within the index of address pointer
				counter++;//Counter is increased to know how many readings there are
			}
			averageHumidity = averageHumidity / counter;//Average is calculated by dividing all humidities by the number of them we looped through
		}
	}
	catch (const out_of_range& error)//Handling the out of range
	{
		cout << "Outside of the 0-24 hour limit" << endl;
	}
	catch (const invalid_argument& error)//Handling invalid argument
	{
		cout << "No sample at the requested time" << endl;
	}
	return averageHumidity;
}
//Method that calculates average temperature looking back over the seconds from current time
double Climate::averageTemperature(long secs)
{
	double averageTemperature = 0; //Variable to store the temperature in
	int counter = 0;

	system_clock::time_point currentTime = std::chrono::system_clock::now();//Getting current time

	std::chrono::duration<double> timeTook = currentTime - startTime; //Getting current second from start time and time now

	try
	{
		int addressPointer = (int)timeTook.count() - secs; //calculating address pointer from current secs - seconds provided

		if (addressPointer > samples.size() || addressPointer <= 0 || addressPointer >= maximum_readings)//If address pointer is more than the size of vector or less than 0
		{
			throw std::out_of_range("");//throw out of range error
		}
		if (samples[addressPointer].temperature == 0)//if temperature of the address pointer is 0, then temperature has not been taken
		{
			throw std::invalid_argument("");//throw invalid argument
		}
		if (samples[addressPointer].temperature > 0 && secs <= samples.size())//If temperature is more than 0 and secs are within range of vector
		{
			for (addressPointer; addressPointer < (int)timeTook.count(); addressPointer++)//Starting from address pointer we loop till the current second
			{
				if (samples[addressPointer].temperature == 0)//if temperature of the address pointer is 0, then temperature has not been taken
				{
					throw std::invalid_argument("");//throw invalid argument
				}
				averageTemperature += Temperature(addressPointer);//Adds the temperature of the reading within the index of address pointer
				counter++;//Counter is increased to know how many readings there are
			}
			averageTemperature = averageTemperature / counter;//Average is calculated by dividing all temperatures by the number of them we looped through
		}
	}
	catch (const out_of_range& error)//Handling the out of range
	{
		cout << "Outside of the 0-24 hour limit" << endl;
	}
	catch (const invalid_argument& error)//Handling invalid argument
	{
		cout << "No sample at the requested time" << endl;
	}
	return averageTemperature;
}