# -*- coding: utf-8 -*-

import numpy as np
import scipy.spatial.distance as dis


class DistanceMatrix:
    
    def setdatafile(self, datafile):
        print 'start computing distance matrix!'
        self.datafile_ = datafile
        print 'datafile: {}'.format(self.datafile_)
        
        
    def loaddata(self):
        print 'loading datafile... ',
        self.data_ = np.loadtxt(self.datafile_, delimiter='\t')
        self.rows_, self.cols_ = self.data_.shape
        print '-> done.'
        print 'data shape: {}'.format(self.data_.shape)
        
        
    def compute(self):
        print 'computing distance matrix... '
        self.dMat_ = np.zeros([self.rows_, self.rows_])
        size = self.rows_**2
        for i in range(self.rows_):
            for j in range(self.rows_):
                print '({}/{})'.format((self.rows_ * i + j), size)
                # distance = 1 - cosine_similarity
                self.dMat_[i, j] = dis.cosine(self.data_[i], self.data_[j])
        print '-> done.'
        
        
    def save(self, filename):
        np.save(filename, self.dMat_)
        print 'saved file: {}'.format(filename)
        

def main():
    dMat = DistanceMatrix()
    dMat.setdatafile('../bin/data/lfw/vgg.tsv')
    dMat.loaddata()
    dMat.compute()
    dMat.save('./dMatrix.npy')


if __name__ == '__main__':
    main()