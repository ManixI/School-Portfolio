#!/usr/python3

import pandas as pd
import numpy as np
from functools import lru_cache
import math

class naive_bayes:
    @lru_cache(maxsize = 128)
    def __init__ (self, data_file):
        self.data = pd.read_csv(data_file)
        self.data = self.data.iloc[:,1:]
        #self.convert_cat_to_bool(self.data, 'legs')
        data = self.data['legs']
        mean = data.sum() / len(data)
        self.data['legs'] = np.where(self.data['legs'] > mean, 1, 0)
        self.train = self.data.sample(frac = 0.7)
        self.test = self.data.drop(self.train.index)
        self.cols = self.train.columns.astype(str)
        values = self.data[self.cols[-1]].unique()
        # LaPlace smoothing value
        self.a = 0.00000001

        # get P(feature)
        prob_feat = []
        for i in self.cols:
            d = self.train[i]
            # prob bool is true is it's mean
            m = d.sum() / len(d)
            prob_feat.append(m)

        # get P(class)
        prob_class = []
        for clas in values:
            t = self.train[self.cols[-1]].value_counts()[clas]
            p = t / len(self.train[self.cols[-1]])
            prob_class.append(p)

        prob = []
        prob_pre = []
        print("-----")
        for i in self.train.index:
            print('-')
            cll = []
            inst = self.train.loc[i]
            for clas in range(len(values)):
                prob_feature = []
                prob_feature.append(np.log(prob_class[clas]))
                for col in self.train.columns.drop(self.cols[-1]):
                    d = inst[col]
                    Pab = np.log(self.con_prob(col))
                    prob_feature.append(Pab)

                for i in range(2,len(prob_feature)):
                    prob_feature[1].add(prob_feature[i], fill_value=0)
                for i in range(len(prob_feature)):
                    prob_feature[i] += prob_feature[0]
                    prob_feature[i] = pow(math.e, prob_feature[i])
                print(prob_feature)
                print()
                tl = prob_feature
                cll.append(tl)

            mi = cll.index(max(cll))
            pre = values[mi]
            prob.append(pre)
            prob_pre.append(cll)
        print(prob_pre)
        print(prob)


    # P(feature | class)
    def con_prob(self, col):
        return (self.train.groupby(col)[self.cols[-1]].value_counts() + self.a) / (self.train.groupby(col)[self.cols[-1]].count()+(self.a * (len(self.cols)-1)))