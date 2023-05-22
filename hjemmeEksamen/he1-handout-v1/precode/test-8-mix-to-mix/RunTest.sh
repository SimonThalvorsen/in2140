# Testfile generator: ./makeTestfiles A X WXY 12018
# Testfile generator: ./makeTestfiles B B XYZ 17012

#!/bin/bash

xterm -ls -e "valgrind --leak-check=full ../proxy 7777" &

read -n 1 -p "Press key when proxy has started."

xterm -ls -e "../anyReceiver W X 127.0.0.1 7777 30" &
sleep 1
xterm -ls -e "../anyReceiver X X 127.0.0.1 7777 30" &
sleep 1
xterm -ls -e "../anyReceiver Y B 127.0.0.1 7777 30" &
sleep 1
xterm -ls -e "../anyReceiver Z B 127.0.0.1 7777 30" &
sleep 5

xterm -ls -e "../binSender B 127.0.0.1 7777" &

../xmlSender A 127.0.0.1 7777

