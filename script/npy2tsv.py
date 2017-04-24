# -*- coding: utf-8 -*-

import numpy as np


def main():
    infile = '../bin/data/cfd/cfd-vgg.npy'
    npydata = np.load(infile)

    outfile = '../bin/data/cfd/cfd-vgg.tsv'
    np.savetxt(outfile, npydata, delimiter='\t')


if __name__ == '__main__':
    main()