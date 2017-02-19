# -*- coding: utf-8 -*-

import numpy as np


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
        self.dMat_ = np.zeros((self.rows_, self.rows_))
        size = self.rows_**2
        for i in range(self.rows_):
            for j in range(self.rows_):
                self.dMat_[i, j] = 1.0 - self.cos_sim(self.data_[i], self.data_[j])
                print '({}/{})'.format((self.rows_ * i + j) + 1, size)
        print '-> done.'
        print self.dMat_
        
        
    def cos_sim(self, v1, v2):
        return np.dot(v1, v2) / (np.linalg.norm(v1) * np.linalg.norm(v2))
        
        
    def save(self, filename):
        np.save(filename, self.dMat_)
        print 'saved file: {}'.format(filename)
        

def main():
    dMat = DistanceMatrix()
    dMat.setdatafile('../sat.tsv')
    dMat.loaddata()
    dMat.compute()
    dMat.save('./dMatrix.npy')


if __name__ == '__main__':
    main()