# -*- coding: utf-8 -*-

"""
各クラス（人）ごとの重心座標
"""

import os
import numpy as np


inputdir = '../bin/data/lfw'


class AveragePoint:

    def __init__(self):
        pass


    def setdatafile(self, datafile, imagefile):
        print 'start computing center point on each class!'
        self.datafile_ = datafile
        self.imagefile_ = imagefile
        print 'datafile: "{}"'.format(self.datafile_)
        print 'imagefile: "{}"'.format(self.imagefile_)
        
        
    def loaddata(self):
        print 'loading datafile.'
        # features data
        self.data_ = np.load(self.datafile_)
        self.rows_, self.cols_ = self.data_.shape

        # class infomation for each feature data
        self.classes_ = np.loadtxt(self.imagefile_, delimiter=' ', dtype=str)[:,1]
        self.classes_ = map(int, self.classes_)
        self.classnum_ = self.classes_[-1] + 1

        print 'data shape: {}'.format(self.data_.shape)
        print 'class number: {}'.format(self.classnum_)
        
        
    def compute_averagepoint(self):
        X = np.zeros((self.classnum_, self.cols_))
        counts = np.zeros(self.classnum_)

        # sum
        for (iclass, idata) in zip(self.classes_, self.data_):
            X[iclass] += idata
            counts[iclass] += 1

        # average
        for i in xrange(len(counts)):
            X[i] = X[i] / counts[i]

        self.average_ = X


    def save(self, filename):
        np.save(filename, self.average_)
        print 'saved file: "{}"'.format(filename)
        

def main():
    inputfile = os.path.join(inputdir, 'lfw-vgg.npy')
    imagesfile = os.path.join(inputdir, 'images.txt')
    outputfile = os.path.join(inputdir, 'lfw-vgg_center.npy')

    center = AveragePoint()
    center.setdatafile(inputfile, imagesfile)
    center.loaddata()
    center.compute_averagepoint()
    center.save(outputfile)


if __name__ == '__main__':
    main()
