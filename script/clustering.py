#!/usr/bin/python2
# -*- coding: utf-8 -*-

import numpy as np
from sklearn.cluster import KMeans
from scipy.spatial.distance import cosine


def main():
    # in/out settings
    inputfile = "/home/yugo/workspace/Interface/bin/data/cfd/cfd-vgg.npy"
    outputfile = "/home/yugo/workspace/Interface/bin/data/cfd/initialize.txt"
    clustersize = 25

    # kmeans clustering
    X = np.load(inputfile)
    kmeans = KMeans(n_clusters=clustersize, init='k-means++', max_iter=1000, n_jobs=-1)
    kmeans.fit(X)
    centers = kmeans.cluster_centers_

    # find nearest sample from centers
    nearest_samples = []
    for center in centers:
        distances = []
        for sample in X:
            distances.append(cosine(center, sample))
        nearest_sample = np.argsort(distances)[0]
        nearest_samples.append(nearest_sample)

    # write file
    np.savetxt(outputfile, np.asarray([nearest_samples]), fmt="%.0f", delimiter=" ")


if __name__ == "__main__":
    main()
