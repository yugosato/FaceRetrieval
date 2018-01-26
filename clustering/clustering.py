#!/usr/bin/python2
# -*- coding: utf-8 -*-

import numpy as np
from sklearn.cluster import KMeans
from scipy.spatial.distance import cosine


class Clustering:
    def __init__(self):
        # in/out settings
        self.inputfile_ = "/home/yugo/workspace/Interface/bin/data/cfd/cfd-vgg.npy"
        self.outputfile_ = "/home/yugo/workspace/Interface/bin/data/cfd/initialize.txt"

    def run(self, method):
        self.features_ = np.load(self.inputfile_)

        if method is "k_means":
            self.cluster_size_ = 25
            k_means = KMeans(n_clusters=self.cluster_size_, init='k-means++', max_iter=1000, n_jobs=-1)
            k_means.fit(self.features_)
            self.centers_ = k_means.cluster_centers_

    def calculate_NN(self):
        nearest_samples = []
        for center in self.centers_:
            distances = []
            for feature in self.features_:
                distances.append(cosine(center, feature))
            nearest_sample = np.argsort(distances)[0]
            nearest_samples.append(nearest_sample)

        np.savetxt(self.outputfile_, np.asarray([nearest_samples]), fmt="%.0f", delimiter=" ")


clustering = Clustering()
clustering.run("k_means")
clustering.calculate_NN()