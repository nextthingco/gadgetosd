#!/bin/bash

kill $(cat gadgetosd.pid) 2>/dev/null && rm gadgetosd.pid
./gadgetosd &
echo $! > gadgetosd.pid

