# -*- coding: utf-8 -*-

import json
import mymodel
import sys
import os
import numpy as np
from chainer import serializers
from dataset import ImportDataset
from libact.base.interfaces import ProbabilisticModel
from libact.query_strategies import UncertaintySampling
from libact.base.dataset import Dataset


home_dir = "/home/yugo/workspace/Interface/trainer"
sys.path.append(home_dir)
os.environ["PATH"] += ":/usr/local/cuda-8.0/bin:/usr/local/cuda-8.0/bin"

class ActiveSelection():

    def __init__(self, listfile, py_settingfile):
        self.listfile_ = listfile
        self.py_settingfile_ = py_settingfile


    def load_dataset(self):
        settings = json.load(open(self.py_settingfile_, "r"))
        self.inputfile_ = settings["input_file"]
        self.unit_ = settings["unit"]
        self.ImpData_ = ImportDataset(self.listfile_, self.inputfile_)
        self.features_ = self.ImpData_.features_


    def load_classifier(self):
        model = mymodel.MyModel(self.unit_)
        model_name = home_dir + "/result/iter-" + str(self.ImpData_.iter_num_ - 1) + "_model.npz"
        print "[ActiveSelection] Load fine-tuned model: {}".format(model_name)
        serializers.load_npz(model_name, model)
        self.clf_ = Chainer2LibAct(model)


    def labeled_index(self):
        positives = self.ImpData_.positives_
        negatives = self.ImpData_.negatives_
        self.labeled_ids_ = np.hstack((positives, negatives))
        self.len_labeled_ids_ = len(self.labeled_ids_)


    def unlabeled_index(self):
        self.unlabeled_ids_ = []
        for i in xrange(len(self.features_)):
            if not i in self.labeled_ids_:
                self.unlabeled_ids_.append(i)
        self.len_unlabeled_ids_ = len(self.unlabeled_ids_)


    def labeled_sample(self):
        Pairs = self.ImpData_.base_
        X = []
        y = []
        for Pair in Pairs:
            X.append(Pair[0])
            y.append(Pair[1])
        return X, y


    def unlabeled_sample(self):
        X = []
        y = [None] * len(self.unlabeled_ids_)
        for unlabeled_id in self.unlabeled_ids_:
            X.append(self.features_[unlabeled_id])
        return X, y


    def split_train_test(self):
        self.labeled_index()
        self.unlabeled_index()
        X_test, y_test = self.labeled_sample()
        X_train, y_train = self.unlabeled_sample()
        tst_ds = Dataset(X_test, y_test)
        trn_ds = Dataset(X_train, y_train)
        return  tst_ds, trn_ds


    def getIndexfromScore(self):
        uncertain_index = []
        for index in self.score_sorted_:
            uncertain_index.append(self.unlabeled_ids_[index[0]])
        return uncertain_index


    def getRandomIndex(self):
        return np.random.choice(self.unlabeled_ids_, self.len_unlabeled_ids_, replace = False)


    def run(self):
        self.load_dataset()
        self.load_classifier()
        _, trn_ds = self.split_train_test()
        qs = UncertaintySampling(trn_ds, method="sm", model=self.clf_)
        _, score = qs.make_query(return_score=True)
        self.score_sorted_ = sorted(score, key=lambda x:x[1], reverse=True)
        uncertain_index = self.getIndexfromScore()
        random_index = self.getRandomIndex()
        self.write(os.path.join(home_dir, "result/uncertain_index.txt"), uncertain_index)
        self.write(os.path.join(home_dir, "result/random_index.txt"), random_index)


    def write(self, filename, index):
        if not os.path.exists(os.path.join(home_dir, "result")):
            os.makedirs(os.path.join(home_dir, "result"))
        np.savetxt(filename, np.array(index), fmt="%.0f")
        print "[ActiveSelection] Saved result. --> {}".format(filename)



class Chainer2LibAct(ProbabilisticModel):

    def __init__(self, model):
        self.model_ = model

    def train(self, x_data):
        return self.model_

    def predict(self, x_data):
        return

    def score(self, x_data):
        return

    def predict_proba(self, x_data):
        X = np.array(x_data, dtype=np.float32)
        y = self.model_.forward(X)
        return y.data


def main():

    # File paths.
    samplelist = "/home/yugo/workspace/Interface/bin/log/feedback.txt"
    py_setting = "/home/yugo/workspace/Interface/bin/data/cfd/py_setting.txt"

    # Active Selection.
    ActSel = ActiveSelection(samplelist, py_setting)
    ActSel.run()


if __name__ == '__main__':
    main()
