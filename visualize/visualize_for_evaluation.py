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
        self.query_log_ = query_log
        self.newinput_ = False
        self.firstinput_ = True
        self.temp_query_ = -1


    def input_check(self):
        if os.path.exists(self.query_log_):
            try:
                self.now_query_ = int(np.loadtxt(self.query_log_)[-1])
            except:
                self.now_query_ = int(np.loadtxt(self.query_log_))

            if self.temp_query_ != self.now_query_:
                self.newinput_ = True
                print "-----------------------------------------------------"
                print "[Mapping] input query image id: {}".format(self.now_query_)
            else:
                self.newinput_ = False
            self.temp_query_ = self.now_query_
        else:
            pass


    def dist(self, x, y):
        X = np.asarray(x)
        Y = np.asarray(y)
        return np.sqrt(np.sum((X - Y) ** 2))


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
        database, = ax.plot(database_x, database_y, database_z, "o", color="b", alpha=0.3, ms=2.0, mew=0.5)

        while True:
            self.input_check()
            if self.newinput_:
                query_x = [X[self.now_query_, 0]]
                query_y = [X[self.now_query_, 1]]
                query_z = [X[self.now_query_, 2]]

                if not self.firstinput_:
                    query.remove()
                    lines, = ax.plot(query_x_past+query_x, query_y_past+query_y, query_z_past+query_z, color="r")
                    distance = self.dist([query_x_past[0],query_x_past[0],query_x_past[0]],[query_x[0],query_y[0],query_z[0]])
                    print "[Mapping] distance: {}".format(distance)
                    print "-----------------------------------------------------"

                query, = ax.plot(query_x, query_y, query_z, "o", color="r", ms=5.0)

                query_x_past = query_x
                query_y_past = query_y
                query_z_past = query_z

                self.firstinput_ = False

            plt.pause(0.00000001)

            # check window opening
            fig_numbers = [x.num for x in plt._pylab_helpers.Gcf.get_all_fig_managers()]
            if len(fig_numbers) == 0:
                break


def main():
    inputdir = "../bin/data/cfd"
    inputfile = "cfd-vgg-tsne_tf.npy"
    query_log = "../bin/log/person.txt"

    mapping = Mapping(inputdir, inputfile, query_log)
    mapping.mapping()


if __name__ == "__main__":
    main()
