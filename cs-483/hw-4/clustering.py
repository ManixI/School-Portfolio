#!/usr/bin/python3

import pandas as pd
import numpy as np
from sklearn.cluster import KMeans
from sklearn.pipeline import Pipeline
from sklearn.model_selection import GridSearchCV
from sklearn.metrics import silhouette_score
from sklearn.preprocessing import MinMaxScaler
import matplotlib.pyplot as plt

data = pd.read_csv('Pokemon.csv')
data.drop(columns=['Type 2'], inplace=True)
#print(data)

data.drop(columns=['Generation'],inplace=True)
xs = data.drop(columns=['Type 1'])
ys = data['Type 1']

scale_elements = ['Total','HP','Attack','Defense','Sp. Atk','Sp. Def','Speed']

types = data['Type 1'].unique()
best_cluster = list()
cluster_data = list()

# itterate over each type
for typ in types:
	k_range = range(2,15)
	# exception for flying type as there are only 5 samples were Type 1 == Flying
	if typ == 'Flying':
		k_range = range(2,4)
	print(typ)
	print('---------------')

	# get all rows with matching type 1
	type_data = data.loc[data['Type 1'] == typ]

	# scale data
	scaler = MinMaxScaler()
	scale = scaler.fit_transform(type_data[scale_elements])
	type_data_scale = pd.DataFrame(scale, columns=scale_elements)

	best_k = 2
	best_score = 0
	for k in k_range:
		cluster = KMeans(k,random_state=0).fit(type_data_scale).labels_
		score = silhouette_score(type_data_scale, cluster,metric="euclidean",random_state=0)
		print(str(k)+' clusters: '+str(score))
		# keep best score
		if best_score < score:
			best_score = score
			best_k = k
			# store set of best cluster to print later
			type_df = pd.DataFrame()
			type_df['data_index'] = type_data.index.values
			type_df['cluster'] = cluster
	print("best num of clusters: "+str(best_k))
	print("best score: "+str(best_score))
	print()
	best_cluster.append(best_k)
	cluster_data.append(type_df)

# print best cluster for eaach type with means
for i in range(len(types)):
	type_data = data.loc[data['Type 1'] == types[i]]
	print(type_data)
	print()
	print(cluster_data[i])
	'''
	print(types[i])
	print("---------------")
	with pd.option_context('display.max_rows',None,'display.max_columns',None):
		for j in range(best_cluster[i]):
			print("Cluster: "+str(j))
			mon_dex = cluster_data[i][cluster_data[i].cluster == j]
			mon_dex = mon_dex['data_index'].tolist()
			#cluster_dat = data.loc[data['#'].isin(mon_dex)]
			cluster_dat = data.iloc[mon_dex]
			cluster_dat = cluster_dat.drop(columns=['#','Type 1', 'Legendary'])
			#cluster_dat = cluster_data.set_index(cluster_dat['#'].tolist())
			print(cluster_dat.to_string())
			print('Mean HP: '+str(cluster_dat['HP'].mean()))
			print('Mean Attack: '+str(cluster_dat['Attack'].mean()))
			print('Mean Defense: '+str(cluster_dat['Defense'].mean()))
			print('Mean Sp. Atk: '+str(cluster_dat['Sp. Atk'].mean()))
			print('Mean Sp. Def:'+str(cluster_dat['Sp. Def'].mean()))
			print('Mean Speed: '+str(cluster_dat['Speed'].mean()))
			print()
	print()
	'''

		