import math
import warnings
import pandas as pd
import numpy as np
from functools import lru_cache
from sklearn.model_selection import train_test_split
from sklearn.model_selection import GridSearchCV
from sklearn.pipeline import Pipeline
from sklearn.base import TransformerMixin, BaseEstimator
from sklearn.metrics import r2_score
from sklearn.metrics import accuracy_score
from sklearn.linear_model import LinearRegression
from sklearn.compose import TransformedTargetRegressor

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


class pipeline():
    # load data into class
    @lru_cache()
    def __init__(self, csvFile, seed):
        pd.set_option('display.max_colwidth', None)

        data = pd.read_csv(csvFile)
        data = data.drop(columns=['Order', 'PID'])
        self.cols = data.drop(columns=['SalePrice']).columns
        self.xs = data.drop(columns = ['SalePrice'])
        self.xs_copy = self.xs
        self.xs = pd.get_dummies(self.xs)
        self.xs.fillna(0, inplace=True)
        
        self.ys = data['SalePrice']
        
        
        if seed != 0:
            np.random.seed(seed)

        self.best = list()
        self.best_accuracy = 0.0

        from sklearn.exceptions import DataConversionWarning
        warnings.filterwarnings(action='ignore', category=RuntimeWarning)




    # returns a list of randomly generated column combonations
    # returns them as a list of tuples so they can be hashed by
    # lru_cache
    @lru_cache()
    def get_col_lists(self, num_tests):
        cols_to_test = list()
        for i in range(num_tests):
            #rand_int = np.random.randint(1,high=(len(self.cols)-1)) # higher accuracy, but maybe no good
            rand_int = np.random.randint(1,high=20)
            tmp_cols = np.random.choice(self.cols, size=rand_int, replace=False)
            tmp_cols = tuple(tmp_cols)
            cols_to_test.append(tmp_cols)
        # only useful if only one loop
        self.list_cols_before_dummies = cols_to_test
        return tuple(cols_to_test)


    # converts list of cols to list of dummy cols
    @lru_cache()
    def convert_to_dummy(self, combos_to_test):
        output = list()
        tmp_list = list()
        combos_to_test = list(combos_to_test)
        #convert tuples back to lists
        for i in range(len(combos_to_test)):
            tmp_list.append(list(combos_to_test[i]))

        # convert from col names to dummy col names
        for i in range(len(combos_to_test)):
            tmp_frame = self.xs_copy[tmp_list[i]]
            tmp_cols = pd.get_dummies(tmp_frame, drop_first=True).columns
            #tmp_cols = tuple(tmp_cols)
            output.append(tmp_cols)

        return output


    @lru_cache()
    def train_test(self, num_tests, outFile):
        # slpit data here so the same data and col lists can be tested multiple times
        train_x, test_x, train_y, test_y = train_test_split( self.xs, self.ys, train_size = 0.7 )

        list_tests = self.get_col_lists(num_tests)
        list_tests = self.convert_to_dummy(list_tests)

        regressor = TransformedTargetRegressor( 
            LinearRegression( n_jobs = -1 ),
            func = np.sqrt,
            inverse_func = np.square 
        ) 

        # dummy values, is there a better way?
        steps = [
            ( 'column_select', SelectColumns( ['Gr Liv Area', 'Overall Qual'] ) ),
            ( 'linear_regression', regressor ),
        ]
        pipe = Pipeline( steps )
        # do I need this?
        pipe.fit(train_x, train_y )

        grid = { 
            'column_select__columns': list_tests, 
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
            search.fit( self.xs, self.ys )
            self.best = search.best_estimator_
            tmp = self.best.get_params
            guess = search.predict(test_x)
            #print("guess:")
            #print(list(test_y))
            #print(type(test_y))
            #self.accuracy = accuracy_score(guess, list(test_y))
            #print()
            #print(self.best.get_feature_names_out())
            #self.best_accuracy = r2_score(list(test_y), list(guess))
            self.score = search.best_score_
            self.best_params = search.best_params_
            #print(list_tests[])
            #print(self.best_accuracy)
            #print(self.score)
            #print("score: "+str(search.best_score_))
            #print("params: "+str(self.best_params_))


            # write test reslut to file 
            if outFile != '':
                with open(outFile, "a") as file:
                    file.write(str(self.best))
                    file.write("\n")
                    file.write(str(self.best_accuracy))
                    file.write("\n")

            return self.score


    # check estimator with different train/test splits
    @lru_cache()
    def best_average(self, best_estimator):
        pass


    def updateSeed(self, seed):
        np.random.seed(seed)