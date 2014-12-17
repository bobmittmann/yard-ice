#!/bin/sh

OUT_DIR="/c/mircom/slcdev-hub/fpga/rs485hub/rs485hub_Implmnt/sbt/outputs/"
BIN_DIR=${OUT_DIR}"/bitmap"
BIN_FILE=${BIN_DIR}"/rs485hub_bitmap.bin"

cp ${BIN_FILE} ./release || {
	echo "Failed!"
	exit 1
}

echo ${BIN_FILE}
echo "Ok."
