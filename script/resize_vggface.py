# -*- coding: utf-8 -*-

import os
from PIL import Image

   
def resize(directory):
    images = os.listdir(directory)
    for image in images:
        print directory, image
        image_path = os.path.join(directory, image)
        Image.open(image_path).resize((224,224)).save(image_path)
    return None
    
    
def walk_resize(images_dir):
    map(lambda f: resize(os.path.join(images_dir, f)), next(os.walk(images_dir))[1])
       
  
if __name__ == '__main__':
    walk_resize('vgg-face-cropped')
