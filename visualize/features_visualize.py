#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import os
import numpy as np
import sklearn.base
import bhtsne
from sklearn import manifold


class BHTSNE(sklearn.base.BaseEstimator, sklearn.base.TransformerMixin):

    def __init__(self, dimensions=2, perplexity=30.0, theta=0.5, rand_seed=-1):
        self.dimensions = dimensions
        self.perplexity = perplexity
        self.theta = theta
        self.rand_seed = rand_seed


    def fit_transform(self, x):
        return bhtsne.tsne(
            x.astype(np.float64), dimensions=self.dimensions, perplexity=self.perplexity,
            theta=self.theta, rand_seed=self.rand_seed)


class MDS(sklearn.base.BaseEstimator):

    def __init__(self, n_componets=2, dissimilarity='euclidean', random_state=6, n_jobs=4):
        self.n_components = n_componets
        self.dissimilarity = dissimilarity
        self.random_state = random_state
        self.n_jobs = n_jobs


    def compute(self, x):
        mds = manifold.MDS(n_components=self.n_components, dissimilarity=self.dissimilarity,
                           random_state=self.random_state, n_jobs=self.n_jobs)
        return mds.fit_transform(x)


def main():

    inputdir = '../bin/data/lfw'
    inputfile = os.path.join(inputdir, 'lfw-vgg_center.npy')

    X = np.load(inputfile)
    tsne = BHTSNE(dimensions=3, perplexity=19.0, theta=10, rand_seed=6)
    Y = tsne.fit_transform(X)

    np.save(os.path.join(inputdir, 'lfw-vgg_center_tsne.npy'), Y)
    print 'data shape: {}'.format(Y.shape)
    print 'complete.'


if __name__ == '__main__':
    main()
