#!/bin/bash

sudo insmod hw5.ko int_str="1,2,3,4,5"
cat /proc/hw5
sudo rmmod hw5.ko
sudo dmesg | tail -n 5
