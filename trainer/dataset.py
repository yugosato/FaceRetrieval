# -*- coding: utf-8 -*-

import json
import numpy as np
import chainer


class ImportDataset(chainer.dataset.DatasetMixin):
    def __init__(self, listfile, inputfilefile):
        self.listfile_ = listfile
        self.inputfile_ = inputfilefile
        self.load()
        self.setsamples()


    def __len__(self):
        return len(self.base_)


    def load(self):
        self.samplelists_ = json.load(open(self.listfile_, "r"))
        self.iter_num_ = len(self.samplelists_)
        self.features_ = np.load(self.inputfile_).astype(np.float32)


    def get_split_sample(self, i):
        positives = self.samplelists_["iter" + str(i)]["positive"]
        negatives = self.samplelists_["iter" + str(i)]["negative"]
        pos_num = len(positives)
        neg_num = len(negatives)

        pairs = []
        if pos_num > 0:
            for pos in positives:
                pairs.append((self.features_[pos], 1))
        if neg_num > 0:
            for neg in negatives:
                pairs.append((self.features_[neg], 0))
        return  pairs, pos_num, neg_num


    def setsamples(self):
        self.base_ = []
        pos_N = 0
        neg_N = 0
        for i in xrange(self.iter_num_):
            pairs, pos_n, neg_n = self.get_split_sample(i)
            self.base_.extend(pairs)
            pos_N += pos_n
            neg_N += neg_n
        print "[Trainer] total sample size: {} (positive: {}, negative: {})".format(len(self.base_), pos_N, neg_N)


    def get_example(self, i):
        vector, label = self.base_[i]
        label = np.array(label, dtype=np.int32)
        return vector, label