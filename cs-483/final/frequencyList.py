#!/bin/user/python3

import pandas as pd
import time
import re

# tree node
class Node:
	def __init__(self, word):
		self.word = word
		self.count = 1
		self.left = None
		self.right = None


	# recursive insert
	def insert(self, word):
		if word == self.word:
			self.count+=1
		elif word < self.word:
			if self.left == None:
				self.left = Node(word)
			else:
				self.left.insert(word)
		else:
			if self.right == None:
				self.right = Node(word)
			else:
				self.right.insert(word)
		return

	# calc IDF for each word
	def calc_freq(self, num_entries):
		self.freq = self.count/num_entries
		if self.left != None:
			self.left.calc_freq(num_entries)
		if self.right != None:
			self.right.calc_freq(num_entries)

	# flatten tree into list
	def flatten(self, final_list):
		if self.left != None:
			self.left.flatten(final_list)
		final_list.append([self.word, self.freq])
		if self.right != None:
			self.right.flatten(final_list)
		return final_list

def transform(xs):
    # remove all HTML tags and lowercase strings
    # remove specificly line breaks, ect.
    def de_tag(t): return re.sub('<br />',' ',t)
    def drop_quote_and_hyphen(t): return re.sub(r"\'|\-", '', t)
    #def drop_puncuation(t): return re.sub('\W', ' ', t)
    def drop_puncuation(t): return re.sub("[^a-zA-Z !?]",'', t)
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

def __sort_helper(ent):
	return(ent[1])

# calc idf for each word in review text and write it to file
def word_freq(data):
	start = time.time()
	print("0 Reading CSV")
	data = pd.read_csv(data)
	data = data.head(n=5000) # shrink data for testing re
	num_entries = len(data)
	#print(num_entries)
	#data = data[['review_text']]

	
	tmp = time.time()
	print(str(round(tmp-start))+" Tokenizing data")
	data = transform(data['review_text'])
	data = data.tolist()
	#print(data[0])
	

	root = None
	tmp = time.time()
	print(str(round(tmp-start))+" Inserting words into tree")
	for i in range(num_entries):
		#doc = data['review_text'].iloc[i]
		#doc = transform(doc)
		doc = data[i]
		doc = doc.split(" ")
		# convert to set to remove duplicates
		doc = set(doc)
		for word in doc:
			if root == None:
				root = Node(word)
			else:
				root.insert(word)

	tmp = time.time()
	print(str(round(tmp-start))+" Calculating IDF for each word")
	root.calc_freq(num_entries)
	idf = list()
	tmp = time.time()
	print(str(round(tmp-start))+" Flatrening tree")
	idf = root.flatten(idf)
	idf.sort(key=__sort_helper, reverse=True)
	tmp = time.time()
	print(str(round(tmp-start))+" Writing to file")
	with open("idf.csv", "w") as fp:
		for word in idf:
			fp.write(str(word[0])+","+str(word[1])+'\n')

word_freq("goodreads_train.csv")
