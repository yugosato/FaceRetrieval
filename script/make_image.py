#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import os
import glob
import sys
import cv2
import json
import numpy as np


imagelist = "/home/yugo/Desktop/dataset/cfd-cropped/images_selected.txt"
images = np.genfromtxt(imagelist, delimiter=' ', dtype='str')

argvs = sys.argv #1. inputdir
argc = len(argvs)

filelists = glob.glob(os.path.join(argvs[1], "candidate_*.txt"))
filelists.sort()

for file in filelists:
    file_basename, _ = os.path.splitext(os.path.basename(os.path.join(argvs[1], file)))
    inputdata = json.load(open(file, "r"))
    iters = len(inputdata)

    if not os.path.exists(os.path.join(argvs[1], file_basename)):
        os.makedirs(os.path.join(argvs[1], file_basename))

    for iter in xrange(iters):
        lists = inputdata["iter" + str(iter)]

        imgs = []
        for list in lists:
            for image in images:
                if int(image[1]) == int(list):
                    imgs.append(cv2.imread(image[0]))

        rowimg = []
        for i in xrange(5):
            rowimg.append(cv2.hconcat(imgs[i*5 : i*5 + 5]))
        dstimg = cv2.vconcat(rowimg[:])

        cv2.imwrite(os.path.join(argvs[1], file_basename, "iter-" + str(iter) + ".jpeg"), dstimg)







