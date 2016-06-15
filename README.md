# GPS-Simulator

By uploading this code onto an arduino, and using an MAX232 to connect to the serial port, you can simulate GPS tracking of the AOML XBT Network.

Open up PuTTY, and upload the code. 

On PuTTY, you will be prompted to enter a date and the path on the XBT Network that you would like to track.

If nothing is entered for a minute, or you merely press "enter" for both inputs, a default date and path will be chosen.

Your location will then be displayed in the GPRMC format approximately every second. 

Close PuTTY, and open OpenCPN next.

Select the Serial Port that you are connected to.

The path of your choice will be on display there and once it travels in each direction 5 times, the next path will begin.
