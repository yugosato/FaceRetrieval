# -*- coding: utf-8 -*-

import matplotlib.pyplot as plt
import cupy as xp
import numpy as np
import random
import chainer
from chainer import training
from chainer import serializers
from chainer import cuda
from sklearn.model_selection import LeaveOneOut
from sklearn.metrics import accuracy_score
import json
import os
import sys
home_dir = "/home/yugo/workspace/Interface/trainer"
sys.path.append(home_dir)
os.environ['PATH'] += ':/usr/local/cuda-8.0/bin:/usr/local/cuda-8.0/bin'
from dataset import ImportDataset
from dataset import SplitImportDataset
from mymodel import MyModel
from mymodel import Chainer2Sklearn


weight = None
bias = None
model = None

def drawGraph(acc, val):
    total_iter_num = 10
    len_acc = len(acc)
    x = [i for i in xrange(total_iter_num)]

    plt.figure(figsize=(11,3))
    plt.fill_between(x[:len_acc], acc - val, acc + val, color="blue", alpha=0.1, label="variance")
    plt.plot(x[:len_acc], acc, color="b", alpha=0.5, label="reliability")
    plt.ylim([0.0, 1.0])
    plt.xlim([0, 10])
    plt.tight_layout(pad=2)
    plt.legend(loc="upper left")
    plt.xlabel("Iteration")
    plt.ylabel("Reliability")
    plt.yticks(np.arange(0.0, 1.1, 0.1))
    plt.xticks(np.arange(0, 11, 1))
    plt.savefig(os.path.join(home_dir, "result", "acc_val.png"))


def variance_score(acc, size):
    return acc * (1 - acc) / size


def remove(dirpath):
    if os.path.exists(dirpath):
        files = os.listdir(dirpath)
        for file in files:
            os.remove(os.path.join(dirpath, file))


def set_random_seed(seed):
    random.seed(seed)
    np.random.seed(seed)
    xp.random.seed(seed)


def train_model():
    # Set Random Seed
    set_random_seed(1)

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
    model = MyModel(unit)

    if gpu_id >= 0:
        cuda.get_device_from_id(gpu_id).use()
        model.to_gpu(gpu_id)

    # Load datasets and set up iterator
    train = ImportDataset(listfile, inputfile)
    train_iter = chainer.iterators.SerialIterator(train, batch_size=batch_size)

    # Set optimizer
    optimizer = chainer.optimizers.AdaDelta()
    optimizer.setup(model)

    # Set up updateer and trainer
    updater = training.StandardUpdater(train_iter, optimizer, device=gpu_id)
    trainer = training.Trainer(updater, (epoch, "epoch"), os.path.join(home_dir, "result"))

    # Slower cause
    # trainer.extend(extensions.LogReport())
    # trainer.extend(extensions.PrintReport(["epoch", "main/loss", "main/accuracy"]))
    # trainer.extend(extensions.PlotReport(["main/loss"], "epoch", file_name="loss.png"))
    # trainer.extend(extensions.PlotReport(["main/accuracy"], "epoch", file_name="accuracy.png"))

    # Run trainer
    print "[Trainer] Start training."
    trainer.run()
    print "[Trainer] Finished training."

    global weight, bias
    weight = model.fc2.W.data
    bias = model.fc2.b.data

    model_name = home_dir + "/result/iter-" + str(train.iter_num_ - 1) + "_model.npz"
    serializers.save_npz(model_name, model)
    print "[Trainer] Saved model. --> {}".format(model_name)


    # Cross-validation testing.
    print "[LOOCV] Start Leave-one-out testing."
    loo = LeaveOneOut()
    split_samples = SplitImportDataset(train.base_)
    true_labels = []
    predicted_labels = []

    for train_index, [test_index] in loo.split(split_samples.input_base_):
        print "[LOOCV] Test: {}, Train: {}.".format(test_index, train_index)

        # Initialize model to train
        model_val = MyModel(unit)

        if gpu_id >= 0:
            cuda.get_device_from_id(gpu_id).use()
            model_val.to_gpu(gpu_id)

        split_samples.split_LOOCV(train_index)
        train_iter_val = chainer.iterators.SerialIterator(split_samples, batch_size=batch_size)

        # Set optimizer
        optimizer_val = chainer.optimizers.AdaDelta()
        optimizer_val.setup(model_val)

        # Set up updateer and trainer
        updater_val = training.StandardUpdater(train_iter_val, optimizer, device=gpu_id)
        trainer_val = training.Trainer(updater_val, (epoch, "epoch"), os.path.join(home_dir, "result"))

        # Run trainer
        trainer_val.run()

        # Testing
        clf = Chainer2Sklearn(model_val)
        X_test = [list(split_samples.input_base_[test_index][0])]

        model_val.to_cpu()
        true_label = int(split_samples.input_base_[test_index][1])
        predicted_label = int(clf.predict(X_test))

        true_labels.append(true_label)
        predicted_labels.append(predicted_label)

    # Calculate accuracy and valiance
    acc = accuracy_score(true_labels, predicted_labels)
    val = variance_score(acc, len(split_samples.input_base_))

    if (train.iter_num_ - 1) == 1:
        if os.path.exists(os.path.join(home_dir, "acc_val.npy")):
            os.remove(os.path.join(home_dir, "acc_val.npy"))
        acc_val = np.array([[0.0], [0.0]])
    else:
        acc_val = np.load(os.path.join(home_dir, "acc_val.npy"))

    # Save as figure.
    acc_val = np.append(acc_val, [[acc], [val]], axis=1)
    np.save(os.path.join(home_dir, "acc_val.npy"), acc_val)
    drawGraph(acc_val[0], acc_val[1])
    print "[LOOCV] Accuracy(average): {}, Variance: {}.".format(acc, val)



def feature_extract(x):
    global model
    X = xp.array([x], dtype=xp.float32)
    y = model.extract(X)
    output = cuda.to_cpu(y[0].data)
    return output
