#!/bin/sh
python quantize.py ../ai8x-training/logs/2023.02.01-180450_NewAlmModel_0_3_ALM65_10k_blur_randScale1.2-0.8/qat_best.pth.tar ../ai8x-training/logs/2023.02.01-180450_NewAlmModel_0_3_ALM65_10k_blur_randScale1.2-0.8/qat_best_q.pth.tar --device MAX78000 -v
