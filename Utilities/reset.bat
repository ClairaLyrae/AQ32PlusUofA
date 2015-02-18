set port=COM14
mode %port%:RTS=off 
mode %port%:DTR=off 
mode %port%:DTR=on
mode %port%:DTR=off 
echo R >%port%:
pause
