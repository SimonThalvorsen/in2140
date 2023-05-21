# Testfile generator: ./makeTestfiles A X X 11133

#!/bin/bash

xterm -ls -hold -e "../proxy 7777" &

read -n 1 -p "Press key when proxy has started."

xterm -ls -hold -e "../anyReceiver X X 127.0.0.1 7777 30" &
sleep 5

../xmlSender A 127.0.0.1 7777

