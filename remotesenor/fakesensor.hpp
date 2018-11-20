#pragma once

#include <random>

class FakeSensor {

private:

		std::random_device rd{};
		std::mt19937 gen{ rd() };
		std::normal_distribution<> h{ 0, 1 };
		std::normal_distribution<> t{ 0, 2 };

protected:

	bool data_valid;

	float temperature;
	float humidity;

	int sensor_status;

	virtual void validate_sensor_data() {};

public:

	FakeSensor() { 
		this->humidity = 55.5 + (float)this->h(gen);
		this->temperature = 18.0 + (float)this->t(gen);
		this->data_valid = true; 
		
	};

	static float convert_c_to_f(float c) { return (c * (float)9 / (float)5 + (float)32); }

	virtual void reset() {};

	virtual void read_data() {
		this->humidity = this->humidity + (float)this->h(gen)/10;
		this->temperature = this->temperature + (float)this->t(gen)/10;
	};

	virtual bool valid() { return this->data_valid; }

	int get_error() { return -1; }

	float get_humidity() { return this->humidity; }

	float get_temperature_in_c() { return this->temperature; }

	float get_temperature_in_f() {
		return this->convert_c_to_f(this->temperature);
	}
};