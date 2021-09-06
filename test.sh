#!/bin/bash

JPEG_DIR=pics
RES_DIR=test
EXEC_NAME=mirror


if [ ! "$(pgrep "$EXEC_NAME")" ]
then
	echo "please, start $EXEC_NAME"
else
	rm -rf "$RES_DIR"
	if [ -d "$JPEG_DIR" ] && [ -f "$EXEC_NAME" ] && [ ! -d "$RES_DIR" ]
	then
		mkdir "$RES_DIR"
		JPEG_IMAGES=$(ls $JPEG_DIR)
		for IMAGE in $JPEG_IMAGES
		do
			curl -s --data-binary "@./$JPEG_DIR/$IMAGE" 127.0.0.1:5875 -o "./$RES_DIR/$IMAGE"
			if [ -f "./$RES_DIR/$IMAGE" ]
			then
				echo "file $IMAGE was succesfully mirrored"
			else
				echo "file $IMAGE failed"
			fi
		done
	fi
fi
