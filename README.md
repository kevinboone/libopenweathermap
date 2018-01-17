libopenweathermap is a simple, GNU C library for fetching and parsing
a 5-day free weather forecast from OpenWeatherMap (OWM). It uses version 2.5 of
the OWM API. To use the API, you will need a "APP ID", which can be obtained
free-of-charge from the OWM website.

libopenweathermap is designed to build on Linux-like systems. It depends on
libcurl for the HTTP operations, and on the standard math library. There should
be no other external dependencies.

The directory test/ includes a simple, command-line test driver that
demonstrates how to use the API to display a three-day forecast. 

Please note that the OWM API is not particular speedy -- it can take
up to a minute to respond. There are daily limits on the number of 
requests that an application can make, and the service may throttle responses.

"make install" will place the library headers in /usr/include/owm, and the
libraries in /usr/lib or /usr/lib64, depending on what architecture is
detected. It may be necessary to tweak the values in the Makefile on
some systems.

This code is copyright (c)2018, Kevin Boone, and distributed according to the 
terms of the GNU public licence,
v3.0.  

Comments, questions, etc to kevin@railwayterrace.com.

