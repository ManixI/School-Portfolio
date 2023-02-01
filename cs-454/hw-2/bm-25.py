#!/usr/bin/python3

import math
import reverseIndex as ri

class BM_25(object):
    """docstring for TF_IDF"""
    def __init__(self, dataFile):
        self.k1 = 1.2
        self.k2 = 500
        self.b = 0.75

        self.dex = ri.reverse_index(dataFile)

    def bm25(self, query, k):
        Q = query.split(" ")
        total = 0
        rel_arr = []
        doc_arr = []
        for word in Q:
            for i in range(len(self.dex.doc_length)):
                #todo: sould only call for docs with term in them
                rel = self.invert_doc_freq(word) * self.term_freq(word, i) * self.query_term_freq(word, query)
                if rel > 0:
                    # if alreayd in array, add relevance of term
                    if i in doc_arr:
                        loc = doc_arr.index(i)
                        rel_arr[loc] += rel
                    # else add term to arr
                    else:
                        rel_arr.append(rel)
                        doc_arr.append(i)

        # convert doc id's to strings
        doc_arr_str = []
        for i in range(len(doc_arr)):
            doc_arr_str.append(str(doc_arr[i]))

        tupl_arr = list(zip(doc_arr_str, rel_arr))
        tupl_arr.sort(reverse=True, key=self.sort_key)
        return tupl_arr[0:k]


    # return invert doc freq of term t, first part of BM25 eq
    # TODO: improve by looking at relevent to term over total relevent instead
    def invert_doc_freq(self, t):
        N = len(self.dex.doc_length)
        loc = self.dex.position(t)
        df_t = len(self.dex.r_dex[loc].doc_id)
        freq = math.log((N-df_t+0.5)/(df_t+0.5), math.e)
        return freq

    # return term portion of bm25 for term t in doc d
    def term_freq(self, t, d):
        loc = self.dex.position(t)
        try:
            loc_2 = self.dex.r_dex[loc].doc_id.index(int(d))
        except:
            return 0
        freq = self.dex.r_dex[loc].count[loc_2]
        length = self.dex.doc_length[int(d)]
        length_average = sum(self.dex.doc_length) / len(self.dex.doc_length)
        tmp = ((self.k1 + 1) * freq)/(self.k1 * ((1-self.b)+self.b*(length/length_average))+freq)
        return tmp

    # retern query term freq part of bm25 for term t in query Q
    def query_term_freq(self, t, Q):
        query = Q.split(" ")
        freq = 0
        for word in query:
            if word == t:
                freq +=1
        tmp = ((self.k2+1)*freq)/(self.k2+freq)
        return freq

    # sort key for sorting returned tuple    
    def sort_key(self, e):
        return e[1]


