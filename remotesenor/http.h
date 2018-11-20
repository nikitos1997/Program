#pragma once

#include "sensor.h"

using namespace System;  // Microsoft Specific

class  Http : public Sensor {

private:
	float Http::arr_to_float(  cli::array<Char>^ Data);
	char* url;

protected:

	void validate_sensor_data();

public:

	Http();

	Http(char * alternate_url);

	void reset();
	void read_data();

protected:

};
