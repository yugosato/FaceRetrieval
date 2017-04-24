# -*- coding: utf-8 -*-

'''
Cropping and Resize lfw dataset
'''

import os
import dlib
import cv2


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
    

def crop_face(directory, out='cropped'):
    person = directory[directory.rfind('\\')+1:]
    images = os.listdir(directory)
   
    for i, image in enumerate(images):
        print image
        image_path = os.path.join(directory, image)
        img = cv2.imread(image_path)
        height, width = img.shape[:2]
        rect = facedetect_dlib(img)
        
        if len(rect) > 0:            
            try:
                os.makedirs(os.path.join(out, person))
            except:
                pass 
            
            for j, nrect in enumerate(rect):
                if not (nrect.left() < 0 or nrect.top() < 0 or nrect.right() > width or nrect.bottom() > height):
                    dst = resize(crop(img, nrect))
                    cv2.imwrite(os.path.join(out, person, person+'_{0:04d}-{1}.jpg'.format(i+1,j+1)), dst)
        else:
            continue
        
    
def walk_crop_face(images_dir):
    map(lambda f: crop_face(os.path.join(images_dir, f)), next(os.walk(images_dir))[1])
   
    
if __name__ == '__main__':
    walk_crop_face('lfw')
