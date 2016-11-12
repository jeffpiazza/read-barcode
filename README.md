# read-barcode
Simple command-line program to return barcode scanner result

This code cribbed from http://stackoverflow.com/questions/29942421/read-barcodes-from-input-event-linux-c.

Run 'make' to compile and link the simple 'barcode' executable.  From the command line, invoke 'barcode' with the barcode device as its single argument, e.g.,
```
barcode /dev/input/by-id/usb-Megawin_Technology_Inc._USB_Keyboard-event-kbd
```
Barcode will wait for input from the barcode reader, then return that input as a string.
