#!/bin/sh

mknod /dev/my_rng_driver c 250 0
ls -l /dev/my_rng_driver

mknod /dev/lkp_enc_driver c 251 0
ls -l /dev/lkp_enc_driver

apt install build-essential zsh
