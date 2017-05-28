# -*- coding: utf-8 -*-

import chainer
from chainer import training
from chainer.training import extensions
from dataset import ImportDataset
import mymodel


def train_model():
    # File paths
    listfile = "/home/yugo/workspace/Interface/bin/log/feedback.txt"
    inputfile = "/home/yugo/workspace/Interface/bin/data/cfd/cfd-vgg.npy"

    # Training parameter
    epoch = 1
    batch_size = 5
    gpu_id = 0

    # Initialize model to train
    model = mymodel.MyModel()
    chainer.datasets.LabeledImageDataset
    if gpu_id >= 0:
        chainer.cuda.get_device_from_id(gpu_id).use()
        model.to_gpu()

    # Load datasets and set up iterator
    train = ImportDataset(listfile, inputfile)
    train_iter = chainer.iterators.SerialIterator(train, batch_size=batch_size)

    # Set optimizer
    optimizer = chainer.optimizers.AdaDelta()
    optimizer.setup(model)

    # # Copy chain with shared parameters to flip 'train' flag only in test
    # eval_model = model.copy()
    # eval_model.train = False

    # Set up updateer and trainer
    updater = training.StandardUpdater(train_iter, optimizer, device=gpu_id)
    trainer = training.Trainer(updater, (epoch, 'epoch'), 'result')
    trainer.extend(extensions.LogReport())
    trainer.extend(extensions.PrintReport(['epoch', 'main/loss', 'main/accuracy']))
    trainer.extend(extensions.PlotReport(['main/loss'], 'epoch', file_name='loss.png'))
    trainer.extend(extensions.PlotReport(['main/accuracy'], 'epoch', file_name='accuracy.png'))

    # Run trainer
    trainer.run()

    print model.fc2.W.data.shape
    print model.fc2.b.data.shape


if __name__ == '__main__':
    train_model()
    