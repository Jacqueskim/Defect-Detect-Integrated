#!/bin/sh
DEVICE="MAX78000"
TARGET="sdk/Examples/$DEVICE/CNN"
COMMON_ARGS="--device $DEVICE --timer 0 --display-checkpoint --verbose"

python ai8xize.py --test-dir $TARGET --prefix AlmModel_0_3_ALM65_10k_blur_randScale1.2-0.8 --checkpoint-file ../ai8x-training/logs/qat_best_q.pth.tar --config-file networks/almond-tinierssd.yaml --sample-input ../ai8x-synthesis/tests/al2.npy --fifo $COMMON_ARGS "$@"