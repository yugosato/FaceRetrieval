#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import os
from sklearn.externals import joblib
import matplotlib.pyplot as plt


inputdir = '../bin/data/lfw'


def main():
    inputfile = os.path.join(inputdir, 'lfw-vgg_center_cos-mds.pkl')

    pos = joblib.load(inputfile)
    plt.scatter(pos[:,0], pos[:,1])
    plt.show()
    
if __name__ == '__main__':
    main()