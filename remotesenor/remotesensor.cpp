/* remotesensor.cpp : main project file.

   requires the following files to be included in a
   windows clr console application to work.

   sensor.h			Abstract class

   http.h			Http sensor class definition
   http.cpp			Http sensor class

   http class contains contains .Net specific code to function
   and therefore cannot be compiled in a linux environment.

   fakesensor.hpp	A sensor class that uses fake values to allow development offline outside of the college network
   remotesensor.cpp	Main program to test the climate.hpp file

   climate.hpp		Your assignment file containing the code for the climate class


*/

#include "stdafx.h" // Global includes options file

#define windows  1   // Set to 1 to compile for windows 0 for Rasp. Pi

#if windows != 1
#include <unistd.h>
#endif

#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <typeinfo> 

#include "climate.hpp"    // TODO: This is the assignment file for you to edit

#define number_of_reads 5 // TODO: Change this to specify how may times to read the sensor

#define number_of_seconds 5

#if windows == 1
using namespace System;  // Microsoft Specific
#endif

using namespace std;

/*
A simple pause function to stop processing for the specified number of seconds.
*/
void pause(int seconds) {
#if windows == 1
	Threading::Thread::Sleep(TimeSpan::FromSeconds(seconds)); // Microsoft Specific Time Delay
#else
	sleep(seconds); //linux specific time delay
#endif	
}

int main()
{

	Climate roomB114;

	int sensorReadTime = 0;

	try { roomB114.clearSamples(); }
	catch (exception &e) { cerr << "Caught: " << e.what() << endl; }

	cout << fixed << setprecision(2);

	pause(1);

	for (int read_counter = 0; read_counter < number_of_reads; read_counter++)
	{
		cout << endl << "Read  " << read_counter + 1;
		try {
			sensorReadTime = roomB114.readSensor();
			
			cout << " reading taken at : " << sensorReadTime << " seconds." << endl;
			
			// TODO: Uncomment lines as you implement in climate.hpp

			cout << " temperature " << roomB114.getTemperature(sensorReadTime) << " C";
			cout << " humidity " << roomB114.getHumidity(sensorReadTime) << "%" << endl;
			pause(1);
		}
		catch (exception &e) {
			cout << " reaing failed." << endl;
			cerr << "Caught: " << e.what() << "Type: " << typeid(e).name() << endl;
		}
	}
	// TODO: Uncomment lines as you implement in climate.hpp
	cout << "Number of samples taken in the last " << number_of_seconds << " seconds " << roomB114.sampleCount(number_of_seconds) << endl;
	cout << "Average Humidity in the last " << number_of_seconds << " seconds " << roomB114.averageHumidity(number_of_seconds) << endl;
	//cout << "Minimum Humidity in the last " << number_of_seconds << " seconds " << roomB114.minimumHumidity(number_of_seconds) << endl;
	//cout << "Maximum Humidity in the last " << number_of_seconds << " seconds " << roomB114.maximumHumidity(number_of_seconds) << endl;
	cout << "Average Temperature in the last " << number_of_seconds << " seconds " << roomB114.averageTemperature(number_of_seconds) << endl;
	//cout << "Minimum Temperature in the last " << number_of_seconds << " seconds " << roomB114.minimumTemperature(number_of_seconds) << endl;
	//cout << "Maximum Temperature in the last " << number_of_seconds << " seconds " << roomB114.maximumTemperature(number_of_seconds) << endl;
	//cout << roomB114("humidity", 1) << ", " << roomB114("temperature", 1) << endl;


#if windows == 1
	system("pause");
#endif
	return(0);
}

