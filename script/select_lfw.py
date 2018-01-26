# -*- coding: utf-8 -*-

import numpy as np
import os


def main():
    inputfile = '/home/yugo/Desktop/dataset/lfw-cropped/images.txt'
    images = np.genfromtxt(inputfile, delimiter=' ', dtype='str')
    directory, _ = os.path.split(inputfile)

    selected = []
    pastdir = None
    for image, id in images:
        dir, _ = os.path.split(image)
        if not dir == pastdir:
            selected.append([image, int(id)])
        pastdir = dir

    selected = np.asarray(selected)
    np.savetxt(os.path.join(directory, 'images_selected.txt'), selected, delimiter=' ', fmt=('%s %s'))


if __name__ == '__main__':
    main()