@echo off
REM Generate input file with all keystrokes
> input.txt echo 1
>> input.txt echo admin
>> input.txt echo 123456
>> input.txt echo 1
>> input.txt echo B001
>> input.txt echo 深入理解计算机系统
>> input.txt echo Randal E. Bryant
>> input.txt echo 计算机
>> input.txt echo 89.00
>> input.txt echo 20
>> input.txt echo 1
>> input.txt echo B002
>> input.txt echo 百年孤独
>> input.txt echo 加西亚·马尔克斯
>> input.txt echo 文学
>> input.txt echo 55.00
>> input.txt echo 15
>> input.txt echo 8
>> input.txt echo 张三
>> input.txt echo 123456
>> input.txt echo 8
>> input.txt echo 李四
>> input.txt echo abcdef
>> input.txt echo 2
>> input.txt echo 0
.\library\book_manager.exe < input.txt
del input.txt
