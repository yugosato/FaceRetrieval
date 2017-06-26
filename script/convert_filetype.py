# -*- coding: utf-8 -*-

import numpy as np
import os


basedir = "../bin/data/cfd/"

def filename(name):
    Name, Ext = os.path.splitext(name)
    return Name

def npy2tsv(infile):
    data = np.load(os.path.join(basedir,infile))
    np.savetxt(os.path.join(basedir, filename(infile)+".tsv"), data, delimiter='\t')

def tsv2npy(infile):
    data = np.loadtxt(os.path.join(basedir,infile))
    np.save(os.path.join(basedir, filename(infile)+".npy"), data)

def main():
    tsv2npy("cfd_histogram.tsv")

if __name__ == '__main__':
    main()