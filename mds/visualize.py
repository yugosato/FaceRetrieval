#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import os
import numpy as np
from sklearn.externals import joblib
import matplotlib.pyplot as plt


def main():
    # mds position
    pos = joblib.load(os.path.join(lfwdir, mdsfile))
    # class for each data sample
    images = np.loadtxt(os.path.join(lfwdir, imglist), delimiter=' ', dtype=str)[:,1]
    images = map(int, images)
    nu=30
    plt.scatter(pos[0:nu,0], pos[0:nu,1])
    
if __name__ == '__main__':
    main()