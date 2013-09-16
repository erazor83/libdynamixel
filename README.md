libdynamixel
============

a speedy C library for accessing Dynamixel AX servos


about
-------------------------
I needed a C library for communication using a simple serial device like USB2AX (http://xevelabs.com/doku.php?id=product:usb2ax:usb2ax).

Another starting point for me has been the dynapi ( http://www.jrl.cs.uni-frankfurt.de/web/projects/dynapi-robotis-dynamixel-c-api/  )
which is in C++ and also only supports the CM5 interface. Which does seem to use ASCII like commands and no raw serial bytes.

So anyway, I started with pydynamixel https://code.google.com/p/pydynamixel/ and decided to write a simple and fast C library.

Here we are.

Most of the code came from libmodbus which to be honest gave me the perfect skeleton for this library.

Thanks again to Stéphane Raimbault from http://libmodbus.org who allowed me to copy and abuse his code.


supported interfaces
-------------------------
  * USB2AX \\ http://xevelabs.com/doku.php?id=product:usb2ax:usb2ax
  * m16u4-dev (using xevelabs firmware) \\ http://ed-solutions.de/dokuwiki/hardware:misc:m16u4:dev


install
-------------------------
All you need is cmake and a gcc. Then simply create makefiles with cmake and feel free to compile:


<pre>
git clone https://github.com/erazor83/libdynamixel
cd libdynamixel
cmake .
make
</pre>


license
-------------------------
GPL-2, so no commercial use is allowed

credits
-------------------------
  * Stéphane Raimbault ( https://github.com/stephane ) \\ for libmodbus
  * Nicolas Saugnier \\ for USB2AX firmware
  * Thomas Volkert ( https://github.com/ThomasVolkert ) \\ for helping me with C and some cmake stuff
  
links
-------------------------

libmodbus
------------
  * http://libmodbus.org
  * https://github.com/stephane/libmodbus

dynapi
------------
  * http://www.jrl.cs.uni-frankfurt.de/web/projects/dynapi-robotis-dynamixel-c-api/

Python
------------
  * http://www.python.org/
  * https://code.google.com/p/pydynamixel/
  