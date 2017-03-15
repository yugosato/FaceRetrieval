#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import os
import numpy as np
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


def embedding_visualization():
    inputfile = os.path.join(inputdir, 'lfw-vgg_center.npy')
    metafile = os.path.join(embeddingdir, 'metadata.tsv')
    logdir = os.path.join(embeddingdir, 'log')

    if not os.path.exists(logdir):
        os.makedirs(logdir)

    features = np.load(inputfile)

    with tf.Session() as sess:
        embedding_var = tf.Variable(features, name='features_embedding')
        config = projector.ProjectorConfig()
        embedding = config.embeddings.add()
        embedding.tensor_name = embedding_var.name
        embedding.metadata_path = metafile
        summary_writer = tf.summary.FileWriter(logdir)

        projector.visualize_embeddings(summary_writer, config)
        sess.run(tf.variables_initializer([embedding_var]))
        saver = tf.train.Saver([embedding_var])
        saver.save(sess, os.path.join(logdir, 'model.ckpt'))


if __name__ == '__main__':
    make_metadata()
    embedding_visualization()