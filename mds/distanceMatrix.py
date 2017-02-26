# -*- coding: utf-8 -*-

import numpy as np
import scipy.spatial.distance as distance

class DistanceMatrix:
    
    def setdatafile(self, datafile):
        print 'start computing cosine distance matrix!'
        self.datafile_ = datafile
        print 'datafile: "{}"'.format(self.datafile_)
        
        
    def loaddata(self):
        print 'loading datafile.'
        self.data_ = np.load(self.datafile_)
        self.rows_, self.cols_ = self.data_.shape
        print 'data shape: {}'.format(self.data_.shape)
        
        
    def compute(self):
        print 'computing distance matrix...'
        self.dMat_ = np.zeros((self.rows_, self.rows_))
        size = self.rows_**2
        
        for i in xrange(self.rows_):
            for j in xrange(self.rows_):
                dis = distance.cosine(self.data_[i], self.data_[j])
                if i == j:
                    dis = 0.0
                self.dMat_[i, j] = dis
                print '{} / {}'.format((self.rows_ * i + j) + 1, size)
#        np.clip(self.dMat_,0,1,self.dMat_)
        
        print 'finished computing distance matrix.'
        print 'dmatrix shape: {}'.format(self.dMat_.shape)
        
        
    def save(self, filename):
        np.save(filename, self.dMat_)
        print 'saved file: "{}"'.format(filename)
        

def main():
    dMat = DistanceMatrix()
    dMat.setdatafile('../bin/data/lfw/lfw-vgg.npy')
    dMat.loaddata()
    dMat.compute()
    dMat.save('./cos-dmatrix.npy')


if __name__ == '__main__':
    main()