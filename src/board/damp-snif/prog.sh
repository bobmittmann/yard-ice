#!/bin/sh

../../../tools/tftp_load.py -q -i -e -r  -a 0x08000000 -h 192.168.10.50 release/damp-snif.bin

