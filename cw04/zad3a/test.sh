#!/usr/bin/env bash
echo "TESTING FOR TYPE: $1"
./catcher "$1" &
PID=$!
sleep 1
./sender "$PID" 1000 "$1"
echo "===================="
