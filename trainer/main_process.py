# -*- coding: utf-8 -*-

import os
import sys
home_dir = "/home/yugo/workspace/Interface/trainer"
sys.path.append(home_dir)
os.environ['PATH'] += ':/usr/local/cuda-8.0/bin:/usr/local/cuda-8.0/bin'
from trainmodel import TrainModel
from active_selection import ActiveSelection


class OnlineProcessing(TrainModel, ActiveSelection):
    def __init__(self):
        super(OnlineProcessing, self).__init__()
        super(TrainModel, self).__init__()
        super(ActiveSelection, self).__init__()


    def run_trainmodel(self):
        self.run_train()
        self.run_feature_extraction()
        self.run_LOOCV()


    def run_activeSelection(self):
        self.run_selection()
        self.run_estimate_class()


def main_process():
    process = OnlineProcessing()
    process.set_random_seed(1)
    process.run_trainmodel()
    process.run_activeSelection()
