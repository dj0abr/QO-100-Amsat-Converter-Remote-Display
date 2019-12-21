# QO-100-Amsat-Converter-Remote-Display
a Linux Web based remote display for any browser. Shows information of the Amsat Up- and Donconverters

# Hardware Requirements
any linux pc and a serial/USB converter with 3.3v level (do NOT use a normal RS-232 USB/serial converter).
runs on PCs or single board computers (i.e. raspberry, odroid and many others).

# planned functionality
* read data from Amsat boards
* fill into a web site
* deliver this website to a bowser using a local apache webserver installation

# this is work in progress.

# Status: 21.Dec 2019
   Downconverter Display: OK and working

# software installation

    1) install apache and php. You can use the script "prepare_apache" to do this. 
    If php is not found then try php7 or similar.
    2) download (clone) all file to a new directory
    3) enter:  
    make
    this compiles this software
    4) enter: 
    sudo make install
    this installs the software and copies the web site files into the webserver directory
    5) start the program by entering:  ./amsatdisplay

# showing the display

the display can be watch in any browser on any system.
Open your favorite web browser and enter the IP address of your linux computer and you will see the display.

# hardware installation

if you own an Amsat-Downconverter V3d or later (it will not work with V1 boards):

The board has an serial output line, located here:

TODO ....


   



