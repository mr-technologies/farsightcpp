# `farsightcpp`

`farsightcpp` is most basic and general C++ sample IFF SDK application.
Application uses C++ API provided by IFF SDK.
It comes with example configuration file (`farsightcpp.json`) demonstrating the following functionality:

* acquisition from XIMEA camera
* writing of raw data to DNG files
* color pre-processing on GPU:
  * black level subtraction
  * histogram calculation
  * white balance
  * demosaicing
  * color correction
  * gamma
  * image format conversion
* automatic control of exposure time and white balance
* H.264 encoding
* RTSP streaming
* HTTP control interface
