# -*- coding: utf-8 -*-

import json
import numpy as np
import chainer


class ImportDataset(chainer.dataset.DatasetMixin):
    def __init__(self, listfile, inputfile):
        self.listfile_ = listfile
        self.inputfile_ = inputfile
        self.load()
        self.setsamples()

    def __len__(self):
        return len(self.base_)


    def load(self):
        self.samplelists_ = json.load(open(self.listfile_, "r"))
        self.iter_num_ = len(self.samplelists_)
        self.features_ = np.load(self.inputfile_).astype(np.float32)


    def get_split_sample(self, i):
        self.positives_ = self.samplelists_["iter" + str(i)]["positive"]
        self.negatives_ = self.samplelists_["iter" + str(i)]["negative"]
        pos_num = len(self.positives_)
        neg_num = len(self.negatives_)

        pairs = []
        if pos_num > 0:
            for pos in self.positives_:
                pairs.append((self.features_[pos], 1))
        if neg_num > 0:
            for neg in self.negatives_:
                pairs.append((self.features_[neg], 0))
        return  pairs, pos_num, neg_num


    def setsamples(self):
        self.base_ = []
        pairs, pos_n, neg_n = self.get_split_sample(self.iter_num_ - 1)
        self.base_ = pairs
        print "[Trainer-Dataset] Total sample size: {} (Positive: {}, Negative: {})".format(len(self.base_), pos_n, neg_n)


    def get_example(self, i):
        vector, label = self.base_[i]
        label = np.array(label, dtype=np.int32)
        return vector, label


class SplitImportDataset(chainer.dataset.DatasetMixin):
    def __init__(self, base):
        self.input_base_ = base

    def split_LOOCV(self, train_index):
        split_pairs = []
        for trn_idx in train_index:
            split_pairs.append(self.input_base_[trn_idx])
        self.base_ = split_pairs

    def __len__(self):
        return len(self.base_)

    def get_example(self, i):
        vector, label = self.base_[i]
        label = np.array(label, dtype=np.int32)
        return vector, label
