# -*- coding: utf-8 -*-

import numpy as np
import cv2



def make_image():
    imagelist = "/home/yugo/Desktop/dataset/lfw-cropped/images_selected.txt"
    names = np.genfromtxt(imagelist, delimiter=' ', dtype='str')

    # load
    imgs = []
    font = cv2.FONT_HERSHEY_PLAIN
    color = (255, 255, 0)
    fontsize = 3
    thickness = 3
    fonttype = cv2.LINE_AA

    for name, number in names:
        print number, name
        img = cv2.imread(name)
        height, width = img.shape[:2]
        cv2.putText(img, number, (5, height - 10), font, fontsize, color, thickness, fonttype)
        imgs.append(img)

    colsize = 40
    remain = len(imgs) % colsize
    size = imgs[0].shape
    black_image = np.zeros(size, dtype=np.uint8)

    if remain > 0:
        for i in xrange(colsize - remain):
            imgs.append(black_image)
    rowsize = len(imgs) / colsize

    # concatenate
    rowimgs = []
    for i in xrange(rowsize):
        rowimgs.append(np.hstack(imgs[i * colsize : i * colsize + colsize]))

    persize = rowsize / 5

    dstimg0 = np.vstack(rowimgs[:persize])
    dstimg1 = np.vstack(rowimgs[persize:2*persize])
    dstimg2 = np.vstack(rowimgs[2*persize:3*persize])
    dstimg3 = np.vstack(rowimgs[3*persize:4*persize])
    dstimg4 = np.vstack(rowimgs[4*persize:])

    # resize
    ratio = 0.3
    height, width = dstimg0.shape[:2]
    dstimg0 = cv2.resize(dstimg0, (int(width * ratio), int(height * ratio)))
    dstimg1 = cv2.resize(dstimg1, (int(width * ratio), int(height * ratio)))
    dstimg2 = cv2.resize(dstimg2, (int(width * ratio), int(height * ratio)))
    dstimg3 = cv2.resize(dstimg3, (int(width * ratio), int(height * ratio)))
    dstimg4 = cv2.resize(dstimg4, (int(width * ratio), int(height * ratio)))

    # write
    cv2.imwrite("imagelist0.png", dstimg0)
    cv2.imwrite("imagelist1.png", dstimg1)
    cv2.imwrite("imagelist2.png", dstimg2)
    cv2.imwrite("imagelist3.png", dstimg3)
    cv2.imwrite("imagelist4.png", dstimg4)


if __name__ == '__main__':
    make_image()
