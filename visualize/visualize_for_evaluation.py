#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import os
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import warnings;warnings.filterwarnings('ignore')


class Mapping:
    def __init__(self, inputdir, init_pos, query_log):
        self.inputdir_ = inputdir
        self.init_pos_ = os.path.join(inputdir, init_pos)
        self.query_log_ = os.path.join(inputdir, query_log)
        self.inputflag_ = False


    def input_check(self):
        if os.path.exists(self.query_log_):
            self.inputflag_ = True
            try:
                self.now_query_ = int(np.loadtxt(self.query_log_)[-1])
            except:
                self.now_query_ = int(np.loadtxt(self.query_log_))
            print "[Mapping] input query image id: {}".format(self.now_query_)
        else:
            pass


    def mapping(self):
        fig = plt.figure(0)
        ax = Axes3D(fig)
        ax.set_xticklabels([])
        ax.set_yticklabels([])
        ax.set_zticklabels([])

        X = np.load(self.init_pos_)

        database_x = X[:, 0]
        database_y = X[:, 1]
        database_z = X[:, 2]

        # plot database
        ax.plot(database_x, database_y, database_z, "o", color="b", alpha=0.2, ms=2.0, mew=0.5)

        while True:
            self.input_check()
            if self.inputflag_:
                query_x = [X[self.now_query_, 0]]
                query_y = [X[self.now_query_, 1]]
                query_z = [X[self.now_query_, 2]]
                query, = ax.plot(query_x, query_y, query_z, "o", color="r", ms=5.0)

            plt.pause(0.00000001)

            if self.inputflag_:
                query.remove()

            # check window opening
            fig_numbers = [x.num for x in plt._pylab_helpers.Gcf.get_all_fig_managers()]
            if len(fig_numbers) == 0:
                break


def main():
    inputdir = '/home/yugo/workspace/Interface/bin/data/lfw'
    inputfile = os.path.join(inputdir, 'lfw-vgg_center-tsne_tf.npy')
    query_log = os.path.join(inputdir, 'person_log.txt')

    mapping = Mapping(inputdir, inputfile, query_log)
    mapping.mapping()


if __name__ == '__main__':
    main()
