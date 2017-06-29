# -*- coding: utf-8 -*-

home_dir = "/home/yugo/workspace/Interface/trainer"
import chainer
from chainer import training
from chainer.training import extensions
from chainer import cuda
import numpy as np
import sys
import os
sys.path.append(home_dir)
os.environ['PATH'] += ':/usr/local/cuda-8.0/bin:/usr/local/cuda-8.0/bin'
from dataset import ImportDataset
import mymodel


# MODE = "VGG"
# MODE = "HISTOGRAM"
# MODE = "GABOR"
MODE = "HISTOGRAM_GABOR"

xp = cuda.cupy

weight = None
bias = None
model = None

def train_model():
    # File paths
    listfile = "/home/yugo/workspace/Interface/bin/log/feedback.txt"

    if MODE == "VGG":
        inputfile = "/home/yugo/workspace/Interface/bin/data/cfd/cfd-vgg.npy"
        unit = 4096
    elif MODE == "HISTOGRAM":
        inputfile = "/home/yugo/workspace/Interface/bin/data/cfd/cfd-histogram.npy"
        unit = 64
    elif MODE == "GABOR":
        inputfile = "/home/yugo/workspace/Interface/bin/data/cfd/cfd-gabor.npy"
        unit = 72
    elif MODE == "HISTOGRAM_GABOR":
        inputfile = "/home/yugo/workspace/Interface/bin/data/cfd/cfd-histogram-gabor.npy"
        unit = 136

    # Training parameter
    epoch = 5
    batch_size = 1
    gpu_id = 0

    print "[Python] epoch: {}".format(epoch)
    print "[Python] mini-batch size: {}".format(batch_size)
    print "[Python] GPU id: {}".format(gpu_id)

    # Initialize model to train
    global model
    model = mymodel.MyModel(unit)

    if gpu_id >= 0:
        chainer.cuda.get_device_from_id(gpu_id).use()
        model.to_gpu(gpu_id)

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
    trainer = training.Trainer(updater, (epoch, "epoch"), os.path.join(home_dir, "result"))
    trainer.extend(extensions.LogReport())
    trainer.extend(extensions.PrintReport(["epoch", "main/loss", "main/accuracy"]))
    trainer.extend(extensions.PlotReport(["main/loss"], "epoch", file_name="loss.png"))
    trainer.extend(extensions.PlotReport(["main/accuracy"], "epoch", file_name="accuracy.png"))

    # Run trainer
    print "[Python] Train Model"
    trainer.run()

    global weight, bias
    weight = model.fc2.W.data
    bias = model.fc2.b.data
    print "[Python] Finished"
    print "================================="


def feature_extract(x):
    global model
    x = xp.array([x], dtype=xp.float32)
    val = chainer.Variable(x)
    y = model.extract(val)
    output = cuda.to_cpu(y[0].data)
    return output
