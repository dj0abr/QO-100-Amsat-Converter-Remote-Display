# QO-100-Amsat-Converter-Remote-Display
a Linux Web based remote display for any browser. Shows information of the Amsat Up- and Donconverters

# Hardware Requirements
any linux pc and a serial/USB converter with 3.3v level (do NOT use a normal RS-232 USB/serial converter).
runs on PCs or single board computers (i.e. raspberry, odroid and many others).

# Functions
* read data from Amsat boards
* fill into a web site
* deliver this website to any bowser using a local apache webserver installation

currently supported boards: Amsat-DL Downconverter V3d

under construction: support of the Amsat-DL Upconverter V2.4

# software installation

    1) install apache and php. You can use the script "prepare_apache" to do this. 
    If php is not found then try php7 or similar.
    2) download (clone) all file to a new directory
    3) personalize the display, see "personalization"
    4) to compile the software enter:  
        make
    5) to install this software enter:
        sudo make install
       this copies the web site files into the webserver directory

# personalization
open the file amsatdisplay.html (in the html folder) with a text editor. Go to the end of the file and look for my callsign: DJ0ABR. 
Replace it with your own callsign and location.

# run this software
start the program by entering:  ./ad

# showing the display

the display can be watched in any browser on any system.
Open your favorite web browser and enter the IP address of your linux computer and you will see the display.

# hardware installation

if you own an Amsat-Downconverter V3d or later (it will not work with V1 boards):

The board has an serial output line, located here:

![alt text](https://www.helitron.de/dj0abr/images/sat/amsat/upconv/a1.png)

this line is also available if the GPS-modul is installed, in this case solder a wire to this pin at the top of the GPS module.

The serial output works with 3,3v level. You need a 3,3v serial/USB converter. You can easily find these converters in the internet for very little money.

    1) connect the 3,3v USB adapter into your linux computer
    2) connect Ground and the serial output of the Amsat-Downconverter with Ground 
          and the serial input (RxD) of your USB adapter.
    3) start this software as described above
    4) switch ON your Amsat Downconverter
    5) start a browser, enter your computer's IP address and watch the remote display
    
Have a lot of fun !
DJ0ABR
