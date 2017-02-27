#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import os
import numpy as np
from sklearn import manifold
from sklearn.externals import joblib


lfwdir = '../bin/data/lfw'
dmatrixfile = 'cos-dmatrix.npy'
resultfile = 'lfw_mds.pkl'


def main():
    # load distance matrix (1 - cosine_similarity)
    X = np.load(os.path.join(lfwdir, dmatrixfile))

    # set mds parameter and fit    
    mds = manifold.MDS(n_components=2, dissimilarity='precomputed', random_state=6, n_jobs=5)
    pos = mds.fit_transform(X)
    
    # serialize the mds model
    joblib.dump(pos, os.path.join(lfwdir, resultfile))


if __name__ == '__main__':
    main()
