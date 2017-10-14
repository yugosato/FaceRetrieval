# -*- coding: utf-8 -*-

import numpy as np
import os


def main():
    basedir = '/home/yugo/Desktop/dataset'
    inputfile_cfd = '/home/yugo/Desktop/dataset/cfd-cropped/images_selected.txt'
    inputfile_lfw = '/home/yugo/Desktop/dataset/lfw-cropped/images_selected.txt'

    images_cfd = np.genfromtxt(inputfile_cfd, delimiter=' ', dtype='str')
    images_lfw = np.genfromtxt(inputfile_lfw, delimiter=' ', dtype='str')

    selected = np.r_[images_cfd, images_lfw]
    for i, select in enumerate(selected):
        select[1] = i
    np.savetxt(os.path.join(basedir, 'images_selected.txt'), selected, delimiter=' ', fmt=('%s %s'))


if __name__ == '__main__':
    main()