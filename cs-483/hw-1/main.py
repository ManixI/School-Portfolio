import pandas as pd
import numpy as np
from functools import lru_cache
import math
import naiveBayes as nb



data = pd.read_csv('zoo.csv')
train = data.sample(frac = 0.7)
test = data.drop(train.index)
a = 0.00000001

# stops pandas from using scientif notation
#pd.options.display.float_format = '{:20,.6f}'.format


'''
#print(train[train.columns[-1]])

cas = train[train.columns[-1]]
#print(cas)
values = cas.unique()
values.sort()
#print(values)
count = cas.value_counts()
#count.sort_values()
#print(count)
#print(count.sum())
#print(values[3])
#print(count[values[4]])
prob = []
for i in range(len(values)):
    tmp = count[values[i]] / count.sum()
    prob.append(tmp)
#print(prob)

clas = train.columns.astype(str)
print(clas)
print(clas[-1])
print("-----")
print(type(clas))
print(clas.values)

rows = train.loc[train[clas[-1]] == 1]
fet_count = rows['hair'].value_counts()
#print("hair counts")
print(fet_count)
#print(fet_count.sum())
#print("-----")
#print(fet_count.iloc[0])
print(fet_count.values)
#print(fet_count.loc[fet_count['legs']==4])
vals = fet_count.index
prob = []
for i in range(len(fet_count)):
    tmp = fet_count.iloc[i] / fet_count.sum()
    prob.append(tmp)
con = pd.Series(prob, index = vals)
print(con)
'''

bayes = nb.naive_bayes('zoo.csv')
'''
print("------ class prob ------")
print(bayes.class_prob(bayes.cols[-1]))
print("------ P(hair) ------")
print(bayes.class_prob(bayes.cols[1]))
print("------ feture prob ------")
print("------ P(hair | class) ------")
tmp = bayes.con_prob('hair')
print(tmp)
#print("---")
#print(tmp.loc[1])
print("------ P(class | 1,2,7) ------")
feats = [bayes.cols[1], bayes.cols[2], bayes.cols[7]]
feats = tuple(feats)
tf = tuple([1,1,1])
tmp = bayes.bays(feats, tf)
print(tmp)
print('-----')
print(tmp.max())
print(tmp.idxmax())

print("------ P(class | 5,12,6) ------")
feats = [bayes.cols[5], bayes.cols[12], bayes.cols[6]]
feats = tuple(feats)
tf = tuple([1,1,1])
tmp = bayes.bays(feats, tf)
print(tmp)
'''