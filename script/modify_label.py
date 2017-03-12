#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import os
import numpy as np


def main():
    images = np.genfromtxt('../bin/data/lfw/images.txt', delimiter=' ', dtype=np.str)
    
    modified = []
    pairs = []
    label = 0
    for i, image in enumerate(images):
        fullpath = image[0]
        delim1 = fullpath.find('lfw-cropped')
        delim2 = fullpath.rfind('/')
        name = fullpath[delim1+12:delim2]
        
        if i == 0:
            pairs.append([label, name])
        elif i > 0:
            if name != past:
                label += 1
                pairs.append([label, name])
        
        modified.append([fullpath,label])
        
        past = name
    
    np.savetxt('../bin/data/lfw/images.txt', modified, delimiter=' ', fmt='%s')
    np.savetxt('../bin/data/lfw/labels.txt', pairs, delimiter=' ', fmt='%s')


if __name__ == '__main__':
    main()