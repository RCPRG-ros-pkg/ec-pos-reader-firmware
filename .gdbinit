target remote | openocd -f interface/ti-icdi.cfg -f board/ek-tm4c123gxl.cfg -c "gdb_port pipe; log_output openocd.log;"
monitor reset halt
