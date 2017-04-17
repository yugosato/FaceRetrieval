#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import os
import numpy as np
import json
import sklearn.base
import bhtsne
from sklearn import manifold


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


# tensorflow projections -> npy file"
def tf2npy(state_path):
    f = open(state_path, 'r')
    json_dict = json.load(f)[0]
    projections = json_dict['projections']

    results = []
    for projection in projections:
        x = projection['tsne-0']
        y = projection['tsne-1']
        z = projection['tsne-2']
        temp = [x, y, z]
        results.append(temp)

    return results


def main():
    inputdir = '../bin/data/lfw'

    # t-SNE
    # inputfile = os.path.join(inputdir, 'lfw-vgg_center.npy')
    # X = np.load(inputfile)
    # tsne = BHTSNE(dimensions=3, perplexity=30.0, theta=0.5, rand_seed=6)
    # Y = tsne.fit_transform(X)

    # # t-SNE (tensorflow)
    inputfile = os.path.join(inputdir, 'state.txt')
    Y = np.asarray(tf2npy(inputfile))

    np.save(os.path.join(inputdir, 'lfw-vgg_center-tsne_tf.npy'), Y)
    print 'data shape: {}'.format(Y.shape)
    print 'complete.'


if __name__ == '__main__':
    main()
