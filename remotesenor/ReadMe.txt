========================================================================
    APPLICATION : remotesenor Project Overview
========================================================================

AppWizard has created this remotesenor Application for you.  

This file contains a summary of what you will find in each of the files that
make up your remotesenor application.

remotesenor.vcxproj
    This is the main project file for VC++ projects generated using an Application Wizard. 
    It contains information about the version of Visual C++ that generated the file, and 
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

remotesenor.vcxproj.filters
    This is the filters file for VC++ projects generated using an Application Wizard. 
    It contains information about the association between the files in your project 
    and the filters. This association is used in the IDE to show grouping of files with
    similar extensions under a specific node (for e.g. ".cpp" files are associated with the
    "Source Files" filter).

AssemblyInfo.cpp
	Contains custom attributes for modifying assembly metadata.

remotesensor.cpp
    This is the main application source file.  There are lines commented out that can be
	uncommented as you implement your climate class.  Line 37 allows you to specify the 
	numer of times the program should read the sensor, keep it at a low value for the 
	start and increase as you feel happy the data is being returned and calculated correctly.

fakesensor.hpp
	A sensor class that provides fake data for development
	Implements the sensor abstract class functionality.

http.h
http.cpp
	A sensor class that provides data from a raspberry pi connected to the college network.
	Implements the sensor abstract class functionality.

sensor.h
	Abstract class that describes the base functionality for any temperature and humidity 
	sensor.

climate.hpp
	Assignment class that implements storing and analysis of sensor data as per the assignment
	brief.


