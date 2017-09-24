# -*- coding: utf-8 -*-

import matplotlib.pyplot as plt
import cupy as xp
import numpy as np
import random
import chainer
from chainer import training
from chainer import cuda
from sklearn.model_selection import LeaveOneOut
from sklearn.metrics import accuracy_score
import json
import os
import sys
import gc
home_dir = "/home/yugo/workspace/Interface/trainer"
sys.path.append(home_dir)
os.environ['PATH'] += ':/usr/local/cuda-8.0/bin:/usr/local/cuda-8.0/bin'
from dataset import ImportDataset
from dataset import SplitImportDataset
from mymodel import MyModel
from mymodel import Chainer2Sklearn
from active_selection import ActiveSelection


class TrainModel():
    def __init__(self):
        # File paths
        self.py_settingfile_ = "/home/yugo/workspace/Interface/bin/data/cfd/py_setting.txt"
        self.settings_ = json.load(open(self.py_settingfile_, "r"))
        self.listfile_ = self.settings_["feedback_file"]
        self.inputfile_ = self.settings_["input_file"]
        self.unit_ = self.settings_["unit"]

        # Training parameter
        self.epoch_ = 5
        self.batch_size_ = 1
        self.gpu_id_ = 0

        # Initialize model
        self.model_ = None


    def drawGraph(self):
        acc = self.acc_val_[0]
        val = self.acc_val_[1]

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


    def variance_score(self, acc, size):
        return acc * (1 - acc) / size


    def remove(self, dirpath):
        if os.path.exists(dirpath):
            files = os.listdir(dirpath)
            for file in files:
                os.remove(os.path.join(dirpath, file))


    def set_random_seed(self, seed):
        random.seed(seed)
        np.random.seed(seed)
        xp.random.seed(seed)


    def run_feature_extraction(self):
        # Extract features
        print "[Trainer] Feature Extraction --> ",
        features = self.model_.extract(xp.array(self.train_.features_))
        features_name = os.path.join(home_dir, "result", "features.tsv")
        np.savetxt(features_name, cuda.to_cpu(features.data), delimiter="\t", fmt="%.18f")
        print features_name


    def run_train(self):
        # Remove old files
        self.remove(os.path.join(home_dir, "result"))

        print "[Trainer] feedback file: \"{}\"".format(self.listfile_)
        print "[Trainer] input: \"{}\"".format(self.inputfile_)
        print "[Trainer] setting: \"{}\"".format(self.py_settingfile_)
        print "[Trainer] dim: {}".format(self.unit_)
        print "[Trainer] epoch: {}".format(self.epoch_)
        print "[Trainer] mini-batch size: {}".format(self.batch_size_)
        print "[Trainer] GPU id: {}".format(self.gpu_id_)

        # Initialize model to train
        self.model_ = MyModel(self.unit_)
        if self.gpu_id_ >= 0:
            cuda.get_device_from_id(self.gpu_id_).use()
            self.model_.to_gpu(self.gpu_id_)

        # Load datasets and set up iterator
        self.train_ = ImportDataset(self.listfile_, self.inputfile_)
        train_iter = chainer.iterators.SerialIterator(self.train_, batch_size=self.batch_size_)

        # Set optimizer
        optimizer = chainer.optimizers.AdaDelta()
        optimizer.setup(self.model_)

        # Set up updateer and trainer
        updater = training.StandardUpdater(train_iter, optimizer, device=self.gpu_id_)
        trainer = training.Trainer(updater, (self.epoch_, "epoch"), os.path.join(home_dir, "result"))

        # Run trainer
        print "[Trainer] Start training --> ",
        trainer.run()
        print "Finished."


    def run_LOOCV(self):
        # Cross-validation testing.
        print "[LOOCV] Start Leave-one-out testing."
        loo = LeaveOneOut()
        split_samples = SplitImportDataset(self.train_.base_)
        true_labels = []
        predicted_labels = []

        for train_index, [test_index] in loo.split(split_samples.input_base_):
            print "[LOOCV] Test: [{}], Train: {}.".format(test_index, train_index)

            # Initialize model to train
            model = MyModel(self.unit_)

            if self.gpu_id_ >= 0:
                cuda.get_device_from_id(self.gpu_id_).use()
                model.to_gpu(self.gpu_id_)

            split_samples.split_LOOCV(train_index)
            train_iter = chainer.iterators.SerialIterator(split_samples, batch_size=self.batch_size_)

            # Set optimizer
            optimizer = chainer.optimizers.AdaDelta()
            optimizer.setup(model)

            # Set up updateer and trainer
            updater = training.StandardUpdater(train_iter, optimizer, device=self.gpu_id_)
            trainer = training.Trainer(updater, (self.epoch_, "epoch"), os.path.join(home_dir, "result"))

            # Run trainer
            trainer.run()

            # Testing
            clf = Chainer2Sklearn(model)
            X_test = [list(split_samples.input_base_[test_index][0])]

            model.to_cpu()
            true_label = int(split_samples.input_base_[test_index][1])
            predicted_label = int(clf.predict(X_test))

            true_labels.append(true_label)
            predicted_labels.append(predicted_label)

        # Calculate accuracy and valiance
        acc = accuracy_score(true_labels, predicted_labels)
        val = self.variance_score(acc, len(split_samples.input_base_))
        print "[LOOCV] Accuracy(average): {}, Variance: {}.".format(acc, val)

        # Save as figure.
        if (self.train_.iter_num_ - 1) == 1:
            if os.path.exists(os.path.join(home_dir, "acc_val.npy")):
                os.remove(os.path.join(home_dir, "acc_val.npy"))
            acc_val = np.array([[0.0], [0.0]])
        else:
            acc_val = np.load(os.path.join(home_dir, "acc_val.npy"))

        self.acc_val_ = np.append(acc_val, [[acc], [val]], axis=1)
        np.save(os.path.join(home_dir, "acc_val.npy"), acc_val)

        self.drawGraph()


def processing():
    # Train Neural Network
    trainer = TrainModel()
    trainer.set_random_seed(1)
    trainer.run_train()
    trainer.run_feature_extraction()
    trainer.run_LOOCV()

    # Active Selection using Trained Model
    actsel = ActiveSelection(trainer)
    actsel.run_selection()
    actsel.run_estimate_class()

    # Memory Release
    del trainer, actsel
    gc.collect()

