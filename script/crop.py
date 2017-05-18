# -*- coding: utf-8 -*-

import os
import dlib
import cv2
import glob


detector = dlib.get_frontal_face_detector()


def crop(img, rect):
    x = rect.left()
    y = rect.top()
    width = rect.right() - x
    height = rect.bottom() - y
    return img[y:y+height, x:x+width]

    
def facedetect_dlib(img):
    cv2.cvtColor(img, cv2.COLOR_BGR2RGB)    
    rect, scores, idx = detector.run(img, 0)
    return rect
        

def resize(img):
    return cv2.resize(img, (224, 224))
    

def crop_face(directory, outdir):
    if not os.path.exists(outdir):
        os.makedirs(outdir)

    persons = os.listdir(directory)
    persons.sort()

    failed = open(os.path.join(directory, 'failed.txt'), 'a')

    for person in persons:
        if not os.path.exists(os.path.join(outdir, person)):
            os.makedirs(os.path.join(outdir, person))

        target = os.path.join(directory, person, '*.*')
        images = glob.glob(target)
        images.sort()

        for image in images:
            print image
            filename = image[image.rfind('/')+1:]
            input_img = cv2.imread(image)
            height, width = input_img.shape[:2]
            rect = facedetect_dlib(input_img)

            if len(rect) > 0:
                for j, nrect in enumerate(rect):
                    if not (nrect.left() < 0 or nrect.top() < 0 or nrect.right() > width or nrect.bottom() > height):
                        dst_img = resize(crop(input_img, nrect))
                        cv2.imwrite(os.path.join(outdir, person, filename), dst_img)
            else:
                failed.writelines(image)
                continue

    
def walk_crop_face(images_dir, outdir):
    map(lambda f: crop_face(os.path.join(images_dir, f), outdir), next(os.walk(images_dir))[1])
   
    
if __name__ == '__main__':
    inputdir = '/home/yugo/Desktop/cfd/CFD Version 2.0.3'
    outdir = '/home/yugo/Desktop/cfd/CFD Version 2.0.3/cropped'
    walk_crop_face(inputdir, outdir)
