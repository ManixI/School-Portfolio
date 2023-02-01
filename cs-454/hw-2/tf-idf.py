#!/usr/bin/python3

import csv
import reverseIndex as rd
import math

# set True for debug print statements
debug = False

class TF_IDF(object):
    #docstring for TF_IDF
    def __init__(self, dataFile):
        self.dex = rd.reverse_index(dataFile)

    def tf_idf(self, Q, k):
        doc_list = []
        rel_arr = []

        query = Q.split(" ")
        # for every doc ger relevance of query
        # TODO: speed up by not checking every doc
        for i in range(len(self.dex.doc_length)):
            rel = self.relevance(i, Q)
            if rel > 0:
                doc_list.append(i)
                rel_arr.append(rel)

        # convert doc id's to strings
        doc_arr_str = []
        for i in range(len(doc_list)):
            doc_arr_str.append(str(doc_list[i]))

        # convert to tupel list and return sorted data
        tupl_arr = list(zip(doc_arr_str, rel_arr))
        tupl_arr.sort(reverse=True, key=self.sort_key)
        return tupl_arr[0:k]

 
    # get relevance of query in specific doc
    def relevance(self, d, Q):
        query = Q.split(" ")
        rel = 0
        for word in query:
            loc = self.dex.position(word)
            length = len(self.dex.r_dex[loc].doc_id)
            rel += self.tf(d, word) / length
            if debug:
                if rel > 0:
                    print("-----")
                    print("rel: "+str(rel))
        count = self.query_count(Q)
        if count == 0:
            return 0
        if debug:
            if rel > 0:
                print("count: "+str(count))
        return rel

    # get tf-idf score for a term in a specific doc
    def tf(self, d, t):
        loc = self.dex.position(t)
        count = self.dex.count(int(d), t)
        try:
            length = self.dex.doc_length[d]
        except:
            print("bad doc: "+str(d))
        tmp = math.log(1 + (count / length), math.e)
        if debug == True:
            if tmp > 0:  
                print("d: "+str(d)+" t: "+t)
                print("count: "+ str(count))
                print("len: "+str(length))
                print("tf: "+ str(tmp))
        return tmp


    # sort key for sorting returned tuple    
    def sort_key(self, e):
        return e[1]

    # returns the numbre of docs with at least one word of Q in it
    def query_count(self, Q):
        query = Q.split(" ")
        count = 0
        docs = []
        # for each term in query
        for term in query:
            loc = self.dex.position(term)
            # get each doc term appears in
            for i in range(len(self.dex.r_dex[loc].doc_id)):
                #if not already in query doc list, add to list
                if self.dex.r_dex[loc].doc_id[i] not in docs:
                    docs.append(self.dex.r_dex[loc].doc_id[i])
        # return length of doc list 
        return len(docs)
