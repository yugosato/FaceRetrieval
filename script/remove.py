# -*- coding: utf-8 -*-

'''
藤さんのデータベースのゴミデータ除去
'''

import os
import shutil
import dlib
import cv2
import hashlib
from PIL import Image


detector = dlib.get_frontal_face_detector()

def facedetect_dlib(path):
    img = cv2.imread(path)
    cv2.cvtColor(img, cv2.COLOR_BGR2RGB)    
    dets, scores, idx = detector.run(img, 0)
    
    if len(dets) > 0:
        return True
    else:
        return False
              
        
def remove_nonface(image_path):           
    if facedetect_dlib(image_path):
        pass
    else:
        os.remove(image_path)
        

def remove_same(directory):
    images = os.listdir(directory)
    fmd5 = []
    dl = []
    for image in images:
        image_path = os.path.join(directory, image)
        with open(image_path, 'rb') as fin:
            data = fin.read()
            m = hashlib.md5(data)
            fmd5.append(m.hexdigest())
        
    for i in xrange(len(images)):
        if images[i] in dl: continue
        for j in xrange(i+1, len(images)):
            if images[j] in dl: continue
            if fmd5[i] == fmd5[j] and not images[j] in dl:
                dl.append(images[j])
    
    count = len(dl)
    for a in dl:
        os.remove(os.path.join(directory, a))
    return count           

    
def convert_channel(image_path):
    data = Image.open(image_path)
    if data.mode != 'RGB':
        try:
	       	data.convert('RGB').save(image_path)
        except IOError:
          	data = 0
           	Image.open(image_path).convert('RGB').save(image_path)
  

def walk(directory):
	print directory
	images = os.listdir(directory)
	for image in images:
		image_path = os.path.join(directory, image)
		convert_channel(image_path)
		remove_nonface(image_path)
	remove_same(directory)
    
def walk_remove_nonface(images_dir):
    map(lambda f: walk(os.path.join(images_dir, f)), next(os.walk(images_dir))[1])

    
if __name__ == '__main__':
    walk_remove_nonface('Fuji-Dataset-B')
