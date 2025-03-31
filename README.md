# rp4-telemetry
This application is designed to run on a raspberry pi 4 that is running the [rp4-yocto](https://github.com/HydrogreenPollub/rp4-yocto) linux distribution.

The purpose of the application is to collect data from `can` and `rs485`, serialize it to a flatbuffer using the [cap'n proto](https://gitlab.com/dkml/ext/c-capnproto) library and send it to a base station via `lora`.

This application also uses the [minmea](https://github.com/kosma/minmea) library for parsing the GPS output.
 
## Build
Before building please notice that the application is hardware specific, so it most likely will not work unless you're using the same peripherals.

If you decide to build the tool anyways, just run the following commands:
``` bash
mkdir build
cd build
cmake ..
make
```