#!/usr/bin/python3

import importlib
import reverseIndex as ri
tfidf = importlib.import_module("tf-idf")
bm25t = importlib.import_module("bm-25")
print("----- test start -----")

file = 'wine.csv'

#index = ri.reverse_index(file)

#print(range(0,len(index.r_dex)-1))
#print(index.r_dex[0].word)
#print(index.r_dex[0].count)
#print(index.count(0,'this'))
#print(index.count(5,'this'))
#print(index.count(6,'this'))
#print(index.count)
#print(index.doc_length[0])
#print(index.r_dex[index.position('tremendous')].doc_id)
#print(index.r_dex[index.position('tremendous')].count)
#print(index.count(73, 'tremendous'))
#print(len(index.r_dex))
#print(len(index.doc_length))


tf = tfidf.TF_IDF('wine.csv')
print('mac watson 5')
tmp = (tf.tf_idf('mac watson', 5))
for i in range(len(tmp)):
    print(tmp[i])
print("----------")
print('tremendous 5')
tmp = (tf.tf_idf('tremendous', 5))
for i in range(len(tmp)):
    print(tmp[i])
print("----------")
print('wine 5')
tmp = (tf.tf_idf('wine', 10))
for i in range(len(tmp)):
    print(tmp[i])
print("----------")
print('tremendous watson 5')
tmp = (tf.tf_idf('tremendous watson', 5))
for i in range(len(tmp)):
    print(tmp[i])
print("----------")
print('tremendous tremendous watson 5')
tmp = (tf.tf_idf('tremendous tremendous watson', 5))
for i in range(len(tmp)):
    print(tmp[i])


#print(tf.relevance(73, "tremendous"))
print("----------")
print("----------")

bm = bm25t.BM_25('wine.csv')
print('mac watson 5')
tmp = bm.bm25('mac watson', 5)
for i in range(len(tmp)):
    print(tmp[i])
print("----------")
print('this 10')
tmp = bm.bm25('this', 10)
for i in range(len(tmp)):
    print(tmp[i])
print("----------")
print('wine 5')
tmp = bm.bm25('wine', 5)
for i in range(len(tmp)):
    print(tmp[i])
print("----------")
print('tremendous watson 5')
tmp = bm.bm25('tremendous watson', 5)
for i in range(len(tmp)):
    print(tmp[i])
print("----------")
print('tremendous tremendous watson 5')
tmp = bm.bm25('tremendous tremendous watson', 5)
for i in range(len(tmp)):
    print(tmp[i])