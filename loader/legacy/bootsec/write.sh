#!/bin/bash

if [ $1 == "" ]
	then echo "Usage:./write.sh [target device]";
else
	size=`stat -c%s bootsec`
	dd if=$1 of=mbr.img bs=1 count=$((512-$size)) skip=$size
	cat bootsec mbr.img > boot.img
	dd if=boot.img of=$1
fi
