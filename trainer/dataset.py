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
        self.neighbors_ = self.samplelists_["iter" + str(i)]["neighbor"]

        len_positive = len(self.positives_)
        len_negative = len(self.negatives_)
        len_neighbor = len(self.neighbors_)

        pairs = []
        if len_positive > 0:
            for index in self.positives_:
                pairs.append((self.features_[index], 1))
        if len_negative > 0:
            for index in self.negatives_:
                pairs.append((self.features_[index], 0))

        self.neighbor_features_ = []
        if len_neighbor > 0:
            for index in self.neighbors_:
                self.neighbor_features_.append(self.features_[index])

        return  pairs, len_positive, len_negative, len_neighbor

    def setsamples(self):
        pairs, positive_num, negative_num, neighbor_num = self.get_split_sample(self.iter_num_ - 1)
        self.base_ = pairs
        print "[Trainer-Dataset] Total sample size: {} (Positive: {}, Negative: {})".format(len(self.base_), positive_num, negative_num)
        print "[Trainer-Dataset] Neighbor samples size: {}".format(neighbor_num)

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
