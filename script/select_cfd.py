# -*- coding: utf-8 -*-

import numpy as np


def main():
    inputfile = '/home/yugo/Desktop/dataset/cfd-cropped/images.txt'
    images = np.genfromtxt(inputfile, delimiter=' ', dtype='str')

    selected = []
    for image, id in images:
        tag = image[image.rfind('.')-1:image.rfind('.')]
        if tag == 'N':
            selected.append([image, int(id)])

    selected = np.asarray(selected)
    np.savetxt('images_selected.txt', selected, delimiter=' ', fmt=('%s %s'))


if __name__ == '__main__':
    main()