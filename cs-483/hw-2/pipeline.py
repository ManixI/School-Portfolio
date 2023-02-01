#! usr/python3

import pandas as pd
import numpy as np
from functools import lru_cache
import math
from itertools import combinations
from sklearn.pipeline import Pipeline
from sklearn.base import TransformerMixin, BaseEstimator
from sklearn.model_selection import train_test_split
from sklearn.model_selection import GridSearchCV
from sklearn.metrics import r2_score
import time
import warnings

class SelectColumns(BaseEstimator, TransformerMixin):
    # pass the function we want to apply to the column 'SalePrice’
    def __init__( self, columns ):
        self.columns = columns
    # don't need to do anything
    def fit( self, xs, ys, **params ):
        return self
    #actually perform the selection
    def transform( self, xs ):
        return xs[ self.columns ]

t = time.time()

data = pd.read_csv('AmesHousing.csv')
data = data.drop(columns=['Order', 'PID'])
#data = pd.get_dummies(data, drop_first=True)
#data = pd.get_dummies(data, columns=['MS SubClass'], drop_first=True)
xs = data.drop( columns = [ 'SalePrice' ] )
ys = data[ 'SalePrice' ]


cols = xs.columns

# set rng seed
seed = 1
np.random.seed(seed)
# rng list of lists of columns
output = list()
l = len(cols)
l += -1
num_loops = 1000
for i in range(int(num_loops/5)):
    r = np.random.randint(1,high=5)
    tmp = np.random.choice(cols, size=r, replace=False)
    '''
    for i in range(len(output)):
        if np.array_equal(tmp, output[i]):
            continue
            '''
    '''
    if any(tmp not in output):
        output.append(tmp)
        print(tmp)
        print(output)
        print()
        '''
    output.append(tmp)

for i in range(int(num_loops/5)):
    r = np.random.randint(1,high=10)
    tmp = np.random.choice(cols, size=r, replace=False)
    #if tmp not in output:
    output.append(tmp)

for i in range(int(num_loops/5)):
    r = np.random.randint(1,high=15)
    tmp = np.random.choice(cols, size=r, replace=False)
    #if tmp not in output:
    output.append(tmp)

for i in range(int(num_loops/5)):
    r = np.random.randint(1,high=20)
    tmp = np.random.choice(cols, size=r, replace=False)
    #if tmp not in output:
    output.append(tmp)

for i in range(int(num_loops/5)):
    r = np.random.randint(1,high=l)
    tmp = np.random.choice(cols, size=r, replace=False)
    #if tmp not in output:
    '''
    for i in range(len(output)):
        if np.array_equal(tmp, output[i]):
            continue
    '''
    output.append(tmp)

# remove dupes
'''
tmp = set(map(tuple, output))
output = list(map(list,output))
'''
'''
output = list(set([tuple(sorted(t))for t in output]))
print(output)
output = list(map(list,output))
'''


#get list of dummy columns from list of columns
output_final = list()
for i in range(num_loops):
    tmp_frame = xs[output[i]]
    tmp_list = pd.get_dummies(tmp_frame).columns
    output_final.append(tmp_list)


train_x, test_x, train_y, test_y = train_test_split( xs, ys, train_size = 0.7 )
train_x = pd.get_dummies(train_x, drop_first=True)
train_x = train_x.fillna(0)
test_x = pd.get_dummies(test_x,drop_first=True)
test_x = test_x.fillna(0)
#train_y = train_y.fillna(0)
#test_y = test_y.fillna(0)


# get regressor to transform target data
from sklearn.linear_model import LinearRegression
from sklearn.compose import TransformedTargetRegressor

regressor = TransformedTargetRegressor( 
    LinearRegression( n_jobs = -1 ),
    func = np.sqrt,
    inverse_func = np.square 
    ) 

steps = [
    ( 'column_select', SelectColumns( ['Gr Liv Area', 'Overall Qual'] ) ),
    ( 'linear_regression', regressor ),
]
pipe = Pipeline( steps )
pipe.fit(train_x, train_y ) 


'''
# make list of all possible column combos

# combos works, but time complexity is too long. Need to A get dummys after list is generated,
# B limit combomations to a smaller list, and C get put in lru_catche somehow
# 4 seems to be limit, need to try RNG approach instest

'''
#output = ['Overall Qual', 'Year Built', 'Gr Liv Area', 'Bedroom AbvGr', 'Overall Cond'], ['Overall Qual', 'Year Built']


grid = { 
    'column_select__columns': output_final, 
        'linear_regression': [LinearRegression( n_jobs = -1 ), # no transformation
        TransformedTargetRegressor(
            LinearRegression( n_jobs = -1 ), 
            func = np.sqrt, inverse_func = np.square ), 
        TransformedTargetRegressor(
            LinearRegression( n_jobs = -1 ), 
            func = np.cbrt, 
            inverse_func = lambda y: np.power( y, 3 ) ), 
        TransformedTargetRegressor(
            LinearRegression( n_jobs = -1 ),
            func = np.log,
            inverse_func = np.exp),] 
}

# suppress warnings where some values are NAN for some things for some reason
# maybe reducing max possible vals would fix, either because refrerencing more cols
# then possible or too many that they round to 0 (most likely)
with warnings.catch_warnings():
    warnings.simplefilter("ignore")
    search = GridSearchCV( pipe, grid, scoring = 'r2', n_jobs = -1 )
    search.fit( train_x, train_y )
    print(search.best_estimator_)
    guess = search.predict(test_x)
    print()
    print("accuracy: "+str(r2_score(list(guess), list(test_y))))
print()
t = time.time() - t
print("time (s) - "+str(t))
print("seed -  "+str(seed))
print("loops - "+str(num_loops))

