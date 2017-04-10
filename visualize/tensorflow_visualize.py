#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import os
import numpy as np
import math
import tensorflow as tf
from tensorflow.contrib.tensorboard.plugins import projector


inputdir = '/home/yugo/workspace/Interface/bin/data/lfw'
embeddingdir = os.path.join(inputdir, 'embedding')


def make_metadata():
    inputfile = os.path.join(inputdir, 'labels.txt')
    labels = np.genfromtxt(inputfile, delimiter=' ', dtype=str)[:,1]

    if not os.path.exists(embeddingdir):
        os.makedirs(embeddingdir)

    metafile = os.path.join(embeddingdir, 'metadata.tsv')

    with open(metafile, 'w') as f:
        for label in labels:
            f.writelines('%s\n' % label)

    print 'embeddings metadata was saved to {}'.format(metafile)


def make_thumbnail_images():
    imagesdir = '/home/yugo/Desktop/tools/lfw-cropped'
    inputfile = os.path.join(inputdir, 'labels.txt')
    labels = np.genfromtxt(inputfile, delimiter=' ', dtype=str)[:, 1]

    thumbnail_images = []
    for label in labels:
        images = os.listdir(os.path.join(imagesdir, label))
        images.sort()
        for image in images:
            fullimage_path = os.path.join(imagesdir, label, image)
            if not os.path.exists(fullimage_path):
                break

            thumbnail_images.append(fullimage_path)
            break

    return thumbnail_images


def embedding_visualization():
    inputfile = os.path.join(inputdir, 'lfw-vgg_center.npy')
    metafile = os.path.join(embeddingdir, 'metadata.tsv')
    logdir = os.path.join(embeddingdir, 'log')

    if not os.path.exists(logdir):
        os.makedirs(logdir)

    features = np.load(inputfile)
    with tf.Session() as sess:
        jpeg_data= tf.placeholder(tf.string)
        thumbnail = tf.cast(tf.image.resize_images(tf.image.decode_jpeg(jpeg_data, channels=3), [100, 100]), tf.uint8)

        images = []
        thumbnail_images = make_thumbnail_images()

        for thumbnail_image in thumbnail_images:
            with open(thumbnail_image, 'rb') as f:
                data = f.read()
                results = sess.run(thumbnail, {jpeg_data: data})
                images.append(results)

        embedding_var = tf.Variable(features, name='features_embedding')
        config = projector.ProjectorConfig()
        embedding = config.embeddings.add()
        embedding.tensor_name = embedding_var.name
        embedding.metadata_path = metafile
        summary_writer = tf.summary.FileWriter(logdir)

        image_path = os.path.join(embeddingdir, 'sprite.jpg')
        size = int(math.sqrt(len(images))) + 1
        while len(images) < size * size:
            images.append(np.zeros((100, 100, 3), dtype=np.uint8))

        rows = []
        for i in range(size):
            rows.append(tf.concat(images[i * size:(i + 1) * size], 1))
        jpeg = tf.image.encode_jpeg(tf.concat(rows, 0))

        with open(image_path, 'wb') as f:
            f.write(sess.run(jpeg))

        embedding.sprite.image_path = image_path
        embedding.sprite.single_image_dim.extend([100, 100])

        projector.visualize_embeddings(summary_writer, config)
        sess.run(tf.variables_initializer([embedding_var]))
        saver = tf.train.Saver([embedding_var])
        saver.save(sess, os.path.join(logdir, 'model.ckpt'))

    print 'embeddings model saved to {}'.format(logdir)


if __name__ == '__main__':
    # make_thumbnail_images()
    # make_metadata()
    embedding_visualization()
