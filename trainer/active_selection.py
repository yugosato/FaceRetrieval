# -*- coding: utf-8 -*-

import json
import numpy as np
from chainer import serializers
from libact.query_strategies import UncertaintySampling
from libact.base.dataset import Dataset
from scipy.spatial.distance import  cosine
import os
import sys
home_dir = "/home/yugo/workspace/Interface/trainer"
sys.path.append(home_dir)
import mymodel
from mymodel import Chainer2Sklearn
from dataset import ImportDataset


class ActiveSelection():
    def __init__(self, py_settingfile):
        self.py_settingfile_ = py_settingfile


    def load_dataset(self):
        settings = json.load(open(self.py_settingfile_, "r"))
        self.listfile_ = settings["feedback_file"]
        self.inputfile_ = settings["input_file"]
        self.unit_ = settings["unit"]
        self.ImpData_ = ImportDataset(self.listfile_, self.inputfile_)
        self.features_ = self.ImpData_.features_


    def load_classifier(self):
        model = mymodel.MyModel(self.unit_)
        model_name = home_dir + "/result/iter-" + str(self.ImpData_.iter_num_ - 1) + "_model.npz"
        print "[ActiveSelection] Load fine-tuned model: {}".format(model_name)
        serializers.load_npz(model_name, model)
        self.clf_ = Chainer2Sklearn(model)


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


    "Random selection for compasion."
    def getRandomIndex(self):
        return np.random.choice(self.unlabeled_ids_, self.len_unlabeled_ids_, replace = False)


    "Traditional active learning method."
    def getUncertaintyIndex(self, trn_ds, method, clf):
        qs = UncertaintySampling(trn_ds, method=method, model=clf)
        _, score = qs.make_query(return_score=True)
        score_sorted = sorted(score, key=lambda x:x[1], reverse=True)
        result = []
        for index in score_sorted:
            result.append(self.unlabeled_ids_[index[0]])
        return result


    "Active distance used in the CueFlik (Fogary+,2008)."
    def getCueFlikIndex(self):
        # Feature extraction using fine-tuned model.
        features = self.clf_.extract(self.features_)

        result = []
        for uid in self.unlabeled_ids_:
            mindist_p = 1.0
            for pos in self.ImpData_.positives_:
                distance = cosine(features[uid], features[pos])
                if distance < mindist_p:
                    mindist_p = distance

            mindist_n = 1.0
            for pos in self.ImpData_.negatives_:
                distance = cosine(features[uid], features[pos])
                if distance < mindist_p:
                    mindist_n = distance

            uncertain = abs(mindist_p - mindist_n)
            active_distance = (mindist_p + mindist_n) * uncertain
            result.append(active_distance)
        return np.argsort(result)


    def run(self):
        self.load_dataset()
        self.load_classifier()
        _, trn_ds = self.split_train_test()

        # Uncertainty Sampling (Margin Sampling)
        uncertain_index = self.getUncertaintyIndex(trn_ds, "sm", self.clf_)
        # CueFlik Sampling
        cueflik_index = self.getCueFlikIndex()
        # Random Sampling
        random_index = self.getRandomIndex()

        self.write(os.path.join(home_dir, "result/active_index.txt"), uncertain_index)
        self.write(os.path.join(home_dir, "result/cueflik_index.txt"), cueflik_index)
        self.write(os.path.join(home_dir, "result/random_index.txt"), random_index)


    def run_estimate_class(self):
        proba = self.clf_.predict_proba(self.features_)
        positive_index = self.sort_positive(proba)
        negative_index = self.sort_negative(proba)
        self.write(os.path.join(home_dir, "result/positive_index.txt"), positive_index)
        self.write(os.path.join(home_dir, "result/negative_index.txt"), negative_index)


    def sort_positive(self, proba):
        positive_proba = []
        for prob in proba:
            positive_proba.append(prob[1])
        return np.argsort(positive_proba)[::-1]


    def sort_negative(self, proba):
        negative_proba = []
        for prob in proba:
            negative_proba.append(prob[0])
        return np.argsort(negative_proba)[::-1]


    def write(self, filename, index):
        if not os.path.exists(os.path.join(home_dir, "result")):
            os.makedirs(os.path.join(home_dir, "result"))
        np.savetxt(filename, np.array(index), fmt="%.0f")
        print "[ActiveSelection] Saved result. --> {}".format(filename)


def active_selection():
    # File paths.
    py_setting = "/home/yugo/workspace/Interface/bin/data/cfd/py_setting.txt"

    # Active Selection.
    ActSel = ActiveSelection(py_setting)
    ActSel.run()
    ActSel.run_estimate_class()
