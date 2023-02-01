#!/bin/user/python3

import math
import warnings
import re
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
from sklearn.tree import DecisionTreeClassifier, plot_tree
from sklearn.ensemble import RandomForestRegressor, RandomForestClassifier
from sklearn.feature_extraction.text import CountVectorizer
from sklearn.preprocessing import Normalizer
from sklearn.linear_model import LogisticRegression

class Preprocessor(BaseEstimator, TransformerMixin):
        def fit(self, xs, ys=None): return self
        def transform(self, xs):
            # remove all HTML tags and lowercase strings
            # remove specificly line breaks, ect.
            def de_tag(t): return re.sub('<br />',' ',t)
            def drop_quote_and_hyphen(t): return re.sub(r"\'|\-", '', t)
            #def drop_puncuation(t): return re.sub('\W', ' ', t)
            def drop_puncuation(t): return re.sub("[^a-zA-Z !?]",'', t)
            #def drop_puncuation(t): return re.sub("[^a-zA-Z ]",'', t)
            def multiple_bang(t): return re.sub(r"(!)\1{1,}", ' $ ',t) # save 2+ ! as symbol $
            def seperate_bang(t): return re.sub(r"!", ' @ ', t)  # replace with different token to avoid infinate loop
            def seperate_que(t): return re.sub(re.escape("?"), ' # ', t)
            def shrink_spaces(t): return re.sub('\s+', ' ', t)
            def shrink_repete(t): return re.sub(r"(.)\1\1+",r"\1\1",t)
            #def drop_comma(t): return re.sub(',','',t)

            transformed = xs.str.lower()
            transformed = transformed.apply(de_tag)
            transformed = transformed.apply(drop_quote_and_hyphen)
            transformed = transformed.apply(drop_puncuation)
            transformed = transformed.apply(seperate_que)
            transformed = transformed.apply(multiple_bang)
            transformed = transformed.apply(seperate_bang)
            transformed = transformed.apply(shrink_spaces)
            transformed = transformed.apply(shrink_repete)
            #transformed = transformed.apply(drop_comma)

            return transformed


def get_stop_words(word_list, min_f, max_f):
    def make_list(word_list, min_f, max_f):
        for i in range(len(word_list)):
            if word_list[i][1] < max_f:
                list1 = word_list[:i]
                break
        
        for i in range(len(word_list)):
            if word_list[i][1] < min_f:
                list2 = word_list[i:]
                break
            else:
                list2 = []
        
        return list1+list2
    def drop_freq(word_list):
        f_list = list()
        for i in range(len(word_list)):
            f_list.append(word_list[i][0])
        return f_list

    word_list = make_list(word_list, min_f, max_f)
    word_list = drop_freq(word_list)
    return word_list
'''
transformed = make_list(word_list, min_f, max_f)
transformed = drop_freq(transformed)

return transformed
'''
data = pd.read_csv("goodreads_train.csv")
#data = data.drop(['user_id','book_id','review_id','date_added','date_updated','read_at','started_at'], axis=1)
xs = data['review_text']
xy = data['rating']

words = pd.read_csv('idf.csv')
word_list = list()
for i in range(len(words)):
    word_list.append([words.iloc[i,0], words.iloc[i,1]])
#print(word_list[0])

vectorizer = \
    CountVectorizer(
        max_df=0.5,
        min_df=3,
        strip_accents='unicode',
        max_features=10000,
        ngram_range=(1,1)           # this is what causes program to run out of mempry
    )

steps = [
    ('tokenize',Preprocessor()),
    ('vectorize',vectorizer),
    ('normalize',Normalizer(norm='l1')), #peanalize words that don't occure frequently
    ('classify',LogisticRegression(solver='sag', verbose=1))
    #('rand_for',RandomForestClassifier(criterion='gini', random_state=0))
]

pipe = Pipeline(steps)
#pipe.fit_transform(xs)
pipe.fit(xs, xy)
print("pipe")


# WARNING: stop list is removing not
grid = {
    'vectorize__stop_words':['english', 
        get_stop_words(word_list, 0, 1),
        #get_stop_words(word_list, 10**-5, 0.3),
        ],
    #'rand_for__n_estimators':[50],
    #'rand_for__max_depth':[20],
}

search = GridSearchCV(pipe, grid, scoring='r2', n_jobs=1, verbose=1)  
#print("grid")
search.fit(xs, xy)
best_e = search.best_estimator_
best_p = search.best_params_
score = search.best_score_

print("best score: "+str(score))
print("best estimato:")
print(best_e)
print("best params: ")
print(best_p)

test_data = pd.read_csv('goodreads_test.csv')
test_features = test_data['review_text']
test_predic = search.predict(test_features)
sub_df = pd.read_csv('goodreads_sample_submission.csv')
sub_df['rating'] = test_predic
sub_df.to_csv("submision.csv", index=False)
print(sub_df['rating'].value_counts())
#print(test_data['rating'].value_counts())