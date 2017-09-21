# -*- coding: utf-8 -*-

home_dir = "/home/yugo/workspace/Interface/trainer"
import chainer
from chainer import training
from chainer.training import extensions
from chainer import serializers
from chainer import cuda
import json
import sys
import os
sys.path.append(home_dir)
os.environ['PATH'] += ':/usr/local/cuda-8.0/bin:/usr/local/cuda-8.0/bin'
from dataset import ImportDataset
import mymodel


xp = cuda.cupy

weight = None
bias = None
model = None

def remove(dirpath):
    if os.path.exists(dirpath):
        files = os.listdir(dirpath)
        for file in files:
            os.remove(os.path.join(dirpath, file))

def train_model():

    # Remove old files
    remove(os.path.join(home_dir, "result"))

    # File paths
    py_settingfile = "/home/yugo/workspace/Interface/bin/data/cfd/py_setting.txt"
    settings = json.load(open(py_settingfile, "r"))
    listfile = settings["feedback_file"]
    inputfile = settings["input_file"]
    unit = settings["unit"]

    # Training parameter
    epoch = 5
    batch_size = 1
    gpu_id = 0

    print "[Trainer] input: \"{}\"".format(inputfile)
    print "[Trainer] setting: \"{}\"".format(py_settingfile)
    print "[Trainer] dim: {}".format(unit)
    print "[Trainer] epoch: {}".format(epoch)
    print "[Trainer] mini-batch size: {}".format(batch_size)
    print "[Trainer] GPU id: {}".format(gpu_id)

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
    print "[Trainer] Start training."
    trainer.run()

    global weight, bias
    weight = model.fc2.W.data
    bias = model.fc2.b.data

    model_name = home_dir + "/result/iter-" + str(train.iter_num_ - 1) + "_model.npz"
    serializers.save_npz(model_name, model)
    print "[Trainer] Saved model."
    print "[Trainer] Finished training."


def feature_extract(x):
    global model
    x = xp.array([x], dtype=xp.float32)
    val = chainer.Variable(x)
    y = model.extract(val)
    output = cuda.to_cpu(y[0].data)
    return output
