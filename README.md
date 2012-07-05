NCRC Activiz
=================

This is an Arduino LED visualization project which will be installed at
the University of Michigan's North Campus Research Complex (NCRC). The Taubman
School of Architecture is designing and installing a collaborative space
at the NCRC which will be complemented with ambient LED visualizations.
These visualizations will be controlled via Arduino and will reflect the
sound and motion within and surrounding the collaborative space.

Parts
-----------

+ [RGB LED Strip - 32 LED/m Addressable](http://www.sparkfun.com/products/10312) 

+ [Electret Microphone Breakout Board](http://www.pololu.com/catalog/product/1620)

+ [PIR Motion Sensor](http://www.sparkfun.com/products/8630)

+ [Arduino UNO SMD](http://arduino.cc/en/Main/ArduinoBoardUnoSMD) 

Dependencies
-----------

+ [FFFT](http://code.google.com/p/neuroelec/downloads/list)
FFT Library for Arduino

+ [LED-Controller](https://github.com/markfickett/LED-Controller)
Controller library for Arduino and Addressable LED Strip

Other Resources
-----------

+ [Arduino Language Reference](http://arduino.cc/it/Reference/HomePage)

+ [Instructables Arduino
  Channel](http://www.instructables.com/tag/type-id/category-technology/channel-arduino/)
  Useful for finding comparable projects

+ [Arduino Forum](http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl)
Community support

Link Dependent Libraries to Arduino's libraries 
-----------------------------------------------
+ Clone the whole project, say /path/to/somewhere/ncrc-activiz
+ Go to Arduino's extended library folder
	In Mac: it should be somewhere like
	/mac/Documents/Arduino/libraries
+ Create folders for the libraries
++ $ mkdir ffft
	$ mkdir ledcontroller	
	$ mkdir NcrcViz
+ Create symbolic link from the libraries in the project folder to Arduino's library folder
	$ ln -s /path/to/somewhere/ncrc-activiz/libraries/ffft/* /mac/Documents/Arduino/libraries/ffft
	$ ln -s /path/to/somewhere/ncrc-activiz/libraries/ledcontroller/* /mac/Documents/Arduino/libraries/ledcontroller
	$ ln -s /path/to/somewhere/ncrc-activiz/libraries/NcrcViz/* /mac/Documents/Arduino/libraries/NcrcViz
