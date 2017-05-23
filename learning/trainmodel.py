# -*- coding: utf-8 -*-

import chainer
from chainer import training
from chainer.training import extensions
from dataset import ImportDataset
import mymodel


def train():
    # File paths
    listfile = "../bin/log/feedback.txt"
    inputfile = "../bin/data/cfd/cfd-vgg.npy"

    # Training parameter
    epoch = 10
    minibatch_size = 5

    # Initialize model to train
    model = mymodel.MyModel()

    # Load datasets and set up iterator
    train = ImportDataset(listfile, inputfile)
    train_iter = chainer.iterators.SerialIterator(train, batch_size=minibatch_size)

    # Set optimizer
    optimizer = chainer.optimizers.Adam()
    optimizer.setup(model)

    # Set up updateer and trainer
    updater = training.StandardUpdater(train_iter, optimizer)
    trainer = training.Trainer(updater, (epoch, 'epoch'), 'result')
    trainer.extend(extensions.LogReport())
    trainer.extend(extensions.PrintReport(['epoch', 'main/loss', 'main/accuracy']))
    trainer.extend(extensions.ProgressBar())

    # Run trainer
    trainer.run()
    

if __name__ == '__main__':
    train()
    