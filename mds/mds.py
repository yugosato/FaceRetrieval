#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import numpy as np
from sklearn import manifold
import matplotlib.pyplot as plt
from sklearn.externals import joblib

def main():
    X = np.load('../bin/data/lfw/cos-dmatrix.npy')    
    mds = manifold.MDS(n_components=2, dissimilarity='precomputed', random_state=6, n_jobs=3)
    pos = mds.fit_transform(X)
    joblib.dump(pos, 'mds.pkl')
    
    for x, y in zip(pos[:,0], pos[:,1]):
        plt.plot(x, y, 'ro')
    plt.savefig('test.png')

if __name__ == '__main__':
    main()
