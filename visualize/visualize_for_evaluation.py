#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import os
import numpy as np
from sklearn.externals import joblib
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D


def main():
    inputdir = '../bin/data/lfw'
    inputfile = os.path.join(inputdir, 'lfw-vgg_center-tsne.npy')
    X = np.load(inputfile)

    fig = plt.figure()
    ax = Axes3D(fig)
    # ax.set_xlabel("X-axis")
    # ax.set_ylabel("Y-axis")
    # ax.set_zlabel("Z-axis")
    ax.set_xticklabels([])
    ax.set_yticklabels([])
    ax.set_zticklabels([])
    ax.plot(X[:, 0], X[:, 1], X[:, 2], "o", color="b", ms=4, mew=0.5)
    plt.show()
    
if __name__ == '__main__':
    main()