# -*- coding: utf-8 -*-

import os
import shutil
import dlib
import cv2
import hashlib
from PIL import Image

detector = dlib.get_frontal_face_detector()


def splitPath(path):
    dpath = os.path.dirname(path)
    base = os.path.basename(path)
    fname, ext = os.path.splitext(base)
    return dpath, fname, ext


def detectGif(path):
    dpath, fname, ext = splitPath(path)
    
    if ext == '.gif':
        return True
    else:
        return False

    
def gif2jpeg(path):
    dpath, fname, ext = splitPath(path)
    image = Image.open(path).convert('RGB')
    image.save(os.path.join(dpath, fname+'.jpeg'))
    os.remove(path)
    

def facedetect_dlib(path):
    img = cv2.imread(path)
    cv2.cvtColor(img, cv2.COLOR_BGR2RGB)    
    dets, scores, idx = detector.run(img, 0)
    
    if len(dets) > 0:
        return True
    else:
        return False
      

def remove_empty(directory):
    print directory
    files = len(os.listdir(directory))
    if files == 0:
        shutil.rmtree(directory)
        return 1
    return 0
        
        
def remove_nonface(directory):
    images = os.listdir(directory)
    count = 0
    for image in images:
        print directory, image,
        image_path = os.path.join(directory, image)
            
        if detectGif(image_path):
            dpath, fname, ext = splitPath(image_path)
            gif2jpeg(image_path)
            image_path = os.path.join(dpath, fname+'.jpeg')    
            
        if facedetect_dlib(image_path):
            print 'is face'
        else:
            os.remove(image_path)
            print 'is not face'
            count += 1
    return count
        

def remove_same(directory):
    images = os.listdir(directory)
    fmd5 = []
    dl = []
    for image in images:
        print directory, image
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

    
def convert_channel(directory):
    images = os.listdir(directory)
    count = 0
    for image in images:
        print directory, image
        image_path = os.path.join(directory, image)
        data = Image.open(image_path)
        if data.mode != 'RGB':
            try:
            	data.convert('RGB').save(image_path)
            except IOError:
            	data = 0
            	Image.open(image_path).convert('RGB').save(image_path)
            count += 1
    return count
    
    
def walk_remove_empty(images_dir):
    x = map(lambda f: remove_empty(os.path.join(images_dir, f)), next(os.walk(images_dir))[1])
    print '# Remove {} empty directories.'.format(sum(x))
    print ''
    
    
def walk_remove_nonface(images_dir):
    x = map(lambda f: remove_nonface(os.path.join(images_dir, f)), next(os.walk(images_dir))[1])
    print '# Remove {} non-face images.'.format(sum(x))
    print ''
    

def walk_remove_same(images_dir):
    x = map(lambda f: remove_same(os.path.join(images_dir, f)), next(os.walk(images_dir))[1])
    print '# Remove {} same-images.'.format(sum(x))
    print ''


def walk_convert_channel(images_dir):
    x = map(lambda f: convert_channel(os.path.join(images_dir, f)), next(os.walk(images_dir))[1])
    print '# Convert {} images to RGB-mode.'.format(sum(x))
    print ''    
    
    
    
if __name__ == '__main__':
#    walk_remove_empty('cropped')
#    walk_remove_nonface('cropped')
#    walk_remove_same('cropped')
    walk_convert_channel('cropped')