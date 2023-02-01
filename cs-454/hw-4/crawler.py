#!/usr/bin/python3

import requests
import urllib.robotparser
import os
import json
import signal as sig
from time import sleep
from urllib.parse import urljoin
from bs4 import BeautifulSoup
from time import time
from sys import exit
#from pandas_multiprocess import multi_process
import pandas as pd
from functools import lru_cache


#TODO: don't let bad URLs get to the que to preserve memory
#TODO: enable multithreading when there is no specified crawl and request rate
#TODO: let program dump memory to file on occasion to prevent balooning memory
#TODO: only get so many lines of the que at once from the file for the same reason
#TODO: JSON should use a nested list for each page, not a flat table


# web crawler class
# @param {string} seed URL
# @param {string} domain to stay within (set None to explore other domains)
class crawler:
    def __init__ (self, seed, domain, num_cores=1, que_file="oldQue.txt", ajacency_file="visited-domains.json",headers=None):
        # ensure file directory exists
        dir_path = os.path.dirname(os.path.realpath(__file__))
        try:
            os.mkdir(dir_path + ' / files')
            print("made files directory")
        except OSError as error:
            print("files directory exists, using existing directory")
        except:
            print("something went wrong") 

        if headers == None:
            self.headers = {"User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/89.0.4389.90 Safari/537.36"}
        else:
            self.headers = headers
        

        # Important: set base info here
        self.seed = seed
        self.domain = domain
        self.num_cores = num_cores
        self.que_file = que_file
        self.ajacency_file = ajacency_file


    # call to update seed domain
    # @param {string} seed domain
    # @param {string} domain to stay within (set None to explore other domains)
    def update_seed(self, seed, domain):
        self.seed = seed
        self.domain = domain

    # call to change name of que file
    # @param {string} que_file
    def update_que_file_name(self, que_file):
        self.que_file = que_file

    # call to change name of ajacency file
    # @param {string} que_file
    def update_que_file_name(self, ajacency_file):
        self.ajacency_file = que_file


    # call to start crawling
    # @param {int} page_limit
    # @param {int} number of cores to run on
    # @param {bool} verbose, print what's being called
    def crawl(self, page_limit, verbose=False):
        if verbose == True:
            start_time = time()
            time_delta = time()

        # get robots.txt
        #TODO: better parsing and use seed not domain
        # add / only if seed dosn't end in /
        rp = urllib.robotparser.RobotFileParser()
        rp.set_url(self.domain+'robots.txt')
        rp.read()
        # max request and call rate, currently unused
        rrate = rp.request_rate("*")
        crate = rp.crawl_delay("*")

        if verbose:
            print("request rate: "+str(rrate))
            print("crawl rate:   "+str(crate))

        # use set to avoid duplicates
        self.que = self.get_old_que()

        # read old que into program if it exists
        try:
            # if file exists and has size > 0 read it into
            if os.stat(os.path.realpath(self.que_file)).st_size != 0:
                with open("oldQue.txt", "r") as fp:
                    for line in fp:
                        self.que.append(line)
                    cursor = self.que.pop()
            else:
                cursor = self.seed
            # open with "w" to wipe file once it's been read
            with open(oldQue.txt, "w") as fp:
                pass
        except:
            cursor = self.seed

        self.ajacent_matrix = self.get_ajacent_from_file()
        visited = self.ajacent_matrix.drop_duplicates(subset=['URL'], keep='first')
        visited = visited['URL'].tolist()

        sig.signal(sig.SIGINT, self.__sig_handler_dump)

        i = 0
        while i < page_limit:
            # delay if required by robots.txt
            if crate != None:
                sleep(crate)
            if rrate != None:
                sleep(rrate)

            # get URLs ajacent to cursor as dataframe
            if cursor not in visited:
                # print current page, time since last page, and total time in seconds
                if verbose:
                    t1 = round((time()-time_delta)*10) / 10
                    t2 = round(time()-start_time)
                    print(str(t1)+" "+str(t2)+" num: "+str(i)+" "+cursor)
                    time_delta = time()
                tmp_set = self.crawl_page(cursor)
                if tmp_set != None:
                    tmp_df = pd.DataFrame({'URL':[], 'Ajacent':[]})
                    for url in tmp_set:
                        tmp = pd.DataFrame({'URL':[cursor], 'Ajacent':[url]})
                        tmp_df = pd.concat([tmp_df,tmp])
                    self.ajacent_matrix = pd.concat([self.ajacent_matrix, tmp_df])
                    tmp_list = tmp_df['Ajacent'].tolist()
                    for ent in tmp_list:
                        self.que.add(ent)
                    # get next page to crawl
                    visited.append(cursor)
                    i+=1

            try:
                # if visited or dissalowed, skip page
                if (self.domain == None):
                    while (cursor in visited) or (not rp.can_fetch("*", cursor)) or (len(cursor) > 255):
                        cursor = self.que.pop()
                        while cursor == None:
                            cursor = self.que.pop()
                else:
                    while  (cursor in visited) or (not rp.can_fetch("*", cursor)) or (self.domain not in cursor) or (len(str(cursor)) > 255):
                        cursor = self.que.pop()
                        while cursor == None:
                            cursor = self.que.pop()
            except:
                print("ERROR! Ending early")
                self.dump(verbose=verbose)
                exit()
        if verbose == True:
            run_time = time() - start_time
            avr_time = page_limit/run_time
            avr_time = round(avr_time+10)/10
            print("Crawl speed: "+str(avr_time)+" pages per second")
        self.dump(verbose)


    def crawl_with_threading(self, page_limit, verbose=False, num_cores=None):
        pass
        if num_cores == None:
            num_cores = self.num_cores



    # crawl a specified page
    # write contents to txt file and return dataframe of ajacent URLs
    # @param {string} page to crawl
    # @return {set} set of ajacent URLs
    def crawl_page(self, cursor):
        #rawPage = requests.get(cursor).txt
        try:
            rawPage = requests.get(cursor, timeout=10, headers=(self.headers)).text
        except:
            print("Error getting page: "+cursor)
            self.dump()

        # load page into beutiful Soup
        bs = BeautifulSoup(rawPage, 'lxml')
        content = bs.get_text(",", strip=True)
        pageName = cursor.replace('/','').replace(':','')
        # write page contents to file
        with open(f'files/{pageName}.txt', 'w') as f:
            f.write(content)

        #array to track ajacent URLs
        ajacent = []

        for link in bs.find_all('a'):
            inQue = False
            visited = False
            stop_dupe = False

            # add all links to que
            # get link and make it absolute if relative
            tmp = link.get('href')
            if tmp and tmp.startswith('/'):
                tmp = urljoin(self.seed, tmp)
            ajacent.append(tmp)
        # cast to set to remove duplicates
        return set(ajacent)

    # get url ajacency matrix from file
    # @return {DataFrame} returns empty file if file is empty
    def get_ajacent_from_file(self):
        try:
            dir_path = os.path.dirname(os.path.realpath(__file__))
            if os.stat(dir_path+"/"+self.ajacency_file).st_size != 0:
                ajacent = pd.read_json(self.ajacency_file)
                return ajacent
        except:
            #TODO find a way to do this that don't require filler to be returned
            tmp = pd.DataFrame({'URL':['filler'], 'Ajacent':['filler']})
            return tmp

    # get old que from file
    # @return {set} old que
    def get_old_que(self):
        s = set()
        try:
            with open(self.que_file, 'r+') as fp:
                for line in fp:
                    s.add(line.strip())
        except:
            pass
        # wipe old file once read
        open(self.que_file,'w').close()
        return s  

    # get domain from seed
    # @param {string} url to get domain from, defaults to seed domain
    # @param {string} domain of seed
    def domain_from_seed(self, seed=None):
        pass
        if seed == None:
            seed = self.seed
        # drop http(s)://www. if part of url

        # drop first / and everything after

        # re-add http(s)://www (is this necessary)

        #return domain


    #dump que and ajacency matrix to file
    def dump(self, verbose=False):
        if verbose == True:
            t = time()
        with open(self.que_file, "w") as fp:
            while len(self.que) > 0:
                s = self.que.pop()
                fp.write(str(s)+'\n')
        visited = self.ajacent_matrix['URL'].tolist()
        visited = set(visited)
        with open(self.ajacency_file, "w") as fp:
            while len(visited) > 0:
                line = self.__df_to_dic(visited.pop())
                json.dump(line, fp)  
        if verbose == True:
            time_delta = time() - t
            print("time to dump: "+str(round(time_delta*10)/10))  
        exit()


    # signal handler to dump que and ajacency matrix to file
    # provides graceful exit on ^C
    # private function
    def __sig_handler_dump(self, signo, frame):
        with open(self.que_file, "w") as fp:
            while len(self.que) > 0:
                s = self.que.pop()
                fp.write(str(s)+'\n')
        visited = self.ajacent_matrix['URL'].tolist()
        visited = set(visited)
        with open(self.ajacency_file, "w") as fp:
            while len(visited) > 0:
                line = self.__df_to_dic(visited.pop())
                json.dump(line, fp)
        exit()

    # convert dataFrame to dict
    # used so only one entry per URL instead of one entry per Ajacent
    def __df_to_dic(self, url):
        tmp = self.ajacent_matrix.loc[self.ajacent_matrix['URL'] == url]
        dic = {
            'URL':url,
            'Ajacent':tmp['Ajacent'].tolist()
        }
        return dic


