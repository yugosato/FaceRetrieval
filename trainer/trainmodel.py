# -*- coding: utf-8 -*-

import random
import numpy as np
import cupy as xp
import matplotlib.pyplot as plt
import seaborn as sns
import chainer
from chainer import training
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


class TrainModel(object):
    def __init__(self):
        # File paths
        self.py_settingfile_ = "/home/yugo/workspace/Interface/bin/data/cfd/py_setting.txt"
        self.settings_ = json.load(open(self.py_settingfile_, "r"))
        self.listfile_ = self.settings_["feedback_file"]
        self.inputfile_ = self.settings_["input_file"]
        self.unit_ = self.settings_["unit"]

        # Training parameter
        self.epoch_ = 10
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

        sns.set_style("darkgrid")
        plt.figure(figsize=(11,3))
        plt.ylim([-0.02, 1.02])
        plt.xlim([0, 10])
        plt.yticks(np.arange(0.0, 1.1, 0.2))
        plt.xticks(np.arange(0, 11, 1))
        plt.fill_between(x[:len_acc], acc - val, acc + val, color="lightcoral", alpha=0.5, label="variance")
        plt.plot(x[:len_acc], acc, color="red", linewidth=1.5, label="reliability")
        plt.xlabel("Iteration")
        plt.ylabel("Reliability")
        plt.tight_layout(pad=2)
        plt.legend(loc="upper left")
        plt.savefig(os.path.join(home_dir, "result", "acc_val.png"))


    @staticmethod
    def variance_score(acc, size):
        return acc * (1 - acc) / size


    @staticmethod
    def remove(dirpath):
        if os.path.exists(dirpath):
            files = os.listdir(dirpath)
            for file in files:
                os.remove(os.path.join(dirpath, file))

    @staticmethod
    def set_random_seed(seed):
        random.seed(seed)
        np.random.seed(seed)
        xp.random.seed(seed)
        print "[Trainer] Set Random Seed: {}.".format(seed)


    def run_feature_extraction(self):
        # Extract features
        print "[Trainer-Extraction] Start feature Extraction."
        if self.gpu_id_ >= 0:
           new_features = self.model_.extract(xp.array(self.train_.features_))
        else:
           new_features = self.model_.extract(np.array(self.train_.features_))

        self.new_features_ = cuda.to_cpu(new_features.data)
        features_name = os.path.join(home_dir, "result", "features.tsv")
        np.savetxt(features_name, self.new_features_, delimiter="\t", fmt="%.18f")
        print "[Trainer-Extraction] --> {}".format(features_name)
        self.model_.to_cpu()


    def run_train(self):
        print "[Trainer-Main] feedback file: \"{}\"".format(self.listfile_)
        print "[Trainer-Main] input: \"{}\"".format(self.inputfile_)
        print "[Trainer-Main] setting: \"{}\"".format(self.py_settingfile_)
        print "[Trainer-Main] dim: {}".format(self.unit_)
        print "[Trainer-Main] epoch: {}".format(self.epoch_)
        print "[Trainer-Main] mini-batch size: {}".format(self.batch_size_)
        print "[Trainer-Main] GPU id: {}".format(self.gpu_id_)

        # Remove old files
        self.remove(os.path.join(home_dir, "result"))

        # Initialize model to train
        model = MyModel(self.unit_)
        if self.gpu_id_ >= 0:
            cuda.get_device_from_id(self.gpu_id_).use()
            model.to_gpu(self.gpu_id_)

        # Load datasets and set up iterator
        self.train_ = ImportDataset(self.listfile_, self.inputfile_)
        train_iter = chainer.iterators.SerialIterator(self.train_, batch_size=self.batch_size_, shuffle=False)

        # Set optimizer
        optimizer = chainer.optimizers.AdaDelta()
        optimizer.setup(model)

        # Set up updateer and trainer
        updater = training.StandardUpdater(train_iter, optimizer, device=self.gpu_id_)
        trainer = training.Trainer(updater, (self.epoch_, "epoch"), os.path.join(home_dir, "result"))

        # Run trainer
        print "[Trainer-Main] Start main training."
        trainer.run()
        self.model_ = model.copy()
        print "[Trainer-Main] --> Finished."


    def run_LOOCV(self):
        # Cross-validation testing.
        print "[Trainer-LOOCV] Start leave-one-out testing."
        loo = LeaveOneOut()
        split_samples = SplitImportDataset(self.train_.base_)
        true_labels = []
        predicted_labels = []

        for train_index, [test_index] in loo.split(split_samples.input_base_):
            print "[Trainer-LOOCV] Test: [{}], Train: {}.".format(test_index, train_index)

            # Initialize model to train
            model_loccv = MyModel(self.unit_)

            if self.gpu_id_ >= 0:
                cuda.get_device_from_id(self.gpu_id_).use()
                model_loccv.to_gpu(self.gpu_id_)

            split_samples.split_LOOCV(train_index)
            train_iter = chainer.iterators.SerialIterator(split_samples, batch_size=self.batch_size_, shuffle=False)

            # Set optimizer
            optimizer = chainer.optimizers.AdaDelta()
            optimizer.setup(model_loccv)

            # Set up updateer and trainer
            updater = training.StandardUpdater(train_iter, optimizer, device=self.gpu_id_)
            trainer = training.Trainer(updater, (self.epoch_, "epoch"), os.path.join(home_dir, "result"))

            # Run trainer
            trainer.run()

            # Testing
            clf = Chainer2Sklearn(model_loccv.to_cpu())
            X_test = [list(split_samples.input_base_[test_index][0])]

            true_label = int(split_samples.input_base_[test_index][1])
            predicted_label = int(clf.predict(X_test))

            true_labels.append(true_label)
            predicted_labels.append(predicted_label)

        # Calculate accuracy and valiance
        acc = accuracy_score(true_labels, predicted_labels)
        val = self.variance_score(acc, len(split_samples.input_base_))
        print "[Trainer-LOOCV] Accuracy (average): {}, Variance: {}.".format(acc, val)

        # Save as figure.
        if (self.train_.iter_num_ - 1) == 1:
            if os.path.exists(os.path.join(home_dir, "acc_val.npy")):
                os.remove(os.path.join(home_dir, "acc_val.npy"))
            acc_val = np.array([[0.0], [0.0]])
        else:
            acc_val = np.load(os.path.join(home_dir, "acc_val.npy"))

        self.acc_val_ = np.append(acc_val, [[acc], [val]], axis=1)
        np.save(os.path.join(home_dir, "acc_val.npy"), self.acc_val_)

        # Draw reriability graph.
        self.drawGraph()

