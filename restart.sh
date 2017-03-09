#!/bin/bash

kill $(cat gadgetosd.pid) 2>/dev/null
./gadgetosd &
echo $! > gadgetosd.pid

