This project demonstrates usage of the DSP Shield Serial Communications API with Matlab

* Contents:
MatlabComm.ino : Main Energia application
serial_array.h/cpp: DSP Shield array transfer communication add-on to the Serial API
MatlabComm.m : Main Matlab application
serial_cmd.m : and the following three files comprise the Matlab serial implementation
serial_connect.m
serial_send_array.m
serial_recv_array.m
DoxygenConfig : Doxygen configuration file
doc : Doxygen output 

* Usage:
1. Load MatlabComm.ino in Energia
2. Open MatlabComm.m in Matlab ensuring all other matlab files are on the path
3. Run MatlabComm.ino program on DSP Shield by loading the binary from Energia
4. After DSP Shield shows "ready" message on OLED display, run the MatlabComm.m in Matlab