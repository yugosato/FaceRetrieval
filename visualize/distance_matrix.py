# -*- coding: utf-8 -*-

"""
距離行列計算（1-cos類似度）
"""

import os
import numpy as np
import scipy.spatial.distance as distance


inputdir = '../bin/data/lfw'


class DistanceMatrix:

    def __init__(self):
        pass

    def setdatafile(self, datafile):
        print 'start computing cosine distance matrix!'
        self.datafile_ = datafile
        print 'datafile: "{}"'.format(self.datafile_)


    def loaddata(self):
        print 'loading datafile.'
        self.data_ = np.load(self.datafile_)
        self.rows_, self.cols_ = self.data_.shape
        print 'data shape: {}'.format(self.data_.shape)


    def compute_distance(self):
        print 'computing distance matrix...'
        X = np.zeros((self.rows_, self.rows_))
        size = self.rows_**2

        for i in xrange(self.rows_):
            for j in xrange(self.rows_):
                dis = distance.cosine(self.data_[i], self.data_[j])
                if i == j:
                    dis = 0.0
                X[i, j] = dis
                print '{} / {}'.format((self.rows_ * i + j) + 1, size)

        self.distance_matrix_ = X

        print 'finished computing distance matrix.'
        print 'distance matrix shape: {}'.format(self.distance_matrix_.shape)


    def save(self, filename):
        np.save(filename, self.distance_matrix_)
        print 'saved file: "{}"'.format(filename)


def main():
    inputfile = os.path.join(inputdir, 'lfw-vgg_center.npy')
    outputfile = os.path.join(inputdir, 'lfw-vgg_center_cos-dmatrix')

    dMat = DistanceMatrix()
    dMat.setdatafile(inputfile)
    dMat.loaddata()
    dMat.compute_distance()
    dMat.save(outputfile)


if __name__ == '__main__':
    main()