set filename=%1%
.\Utilities\dfu-util.exe --reset --device 0483:df11 --alt 0 --download %filename%
