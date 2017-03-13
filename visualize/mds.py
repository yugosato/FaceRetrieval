#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import os
import numpy as np
from sklearn import manifold
from sklearn.externals import joblib


inputdir = '../bin/data/lfw'


def main():
    inputfile = os.path.join(inputdir, 'lfw-vgg_center_cos-dmatrix.npy')
    outputfile = os.path.join(inputdir, 'lfw-vgg_center_cos-mds.pkl')

    X = np.load(inputfile)
    mds = manifold.MDS(n_components=2, dissimilarity='precomputed', random_state=6, n_jobs=5)
    pos = mds.fit_transform(X)
    joblib.dump(pos, outputfile)


if __name__ == '__main__':
    main()
