#!/usr/bin/python3

import requests
import urllib.robotparser
import os
import json
from time import sleep
from urllib.parse import urljoin
from bs4 import BeautifulSoup

# TODO: dump que to file on graceful exit
# so crawler can start where it left off and not get stuck if re-started a bunch
# don't use que, use existing ajacency matrix to find uncrawled pages
# TODO: add to encs gitLab
# TODO: phase out example entry


# sample crawling script provided in class
# modified for this project
def main():
    # ensure file directory exists
    dir_path = os.path.dirname(os.path.realpath(__file__))
    try:
        os.mkdir(dir_path + ' / files')
    except OSError as error:
        print("files directory exists, using existing directory") 
    try:   
        pageLimit = int(input("How many pages would you like to crawl?\n"))
    except:
        #default page limit
        pageLimit = 5000
    if pageLimit == '' or pageLimit < 1:
        pageLimit = 5000

    seed = 'https://www.wikipedia.org/'
    default = input("Would you like to use the default seed of: https://www.wikipedia.org/? (y/n)\n")
    if default == "n":
        seed = input("Ender Seed: ")
    # domain currently being checked
    cursor = seed

    with open("visited-domains.json", "a+") as jf:
        #if visited domains file is empty
        if os.stat(dir_path+"/visited-domains.json").st_size == 0:
            #initalize it
            # first row contains example entry
            init = '''{
            "sites_visited": [
                    {
                        "url": "example",
                        "ajacent": [
                            "sites",
                            "go",
                            "here"
                        ]
                    }
                ]
            }'''
            jf.write(init)

    # get robots.txt
    rp = urllib.robotparser.RobotFileParser()
    rp.set_url(seed+'robots.txt')
    rp.read()
    # max request and call rate, currently unused
    rrate = rp.request_rate("*")
    crate = rp.crawl_delay("*")

    que = []
                    
    i = 0
    while i < pageLimit:
        # crawl rate limiter
        if crate and rrate:
            if crate > rrate:
                sleep(crate)
            else:
                sleep(rrate)
        elif crate != None:
            sleep(crate)
        elif rrate!= None:
            sleep(rrate)

        try:
            rawPage = requests.get(cursor).text
        except:
            cursor = que.pop(0)
            continue
        bs = BeautifulSoup(rawPage, 'lxml')
        content = bs.get_text(",", strip=True)
        pageName = cursor.replace('/','').replace(':','')
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
                tmp = urljoin(seed, tmp)
            ajacent.append(tmp)

            # check if already visited
            with open("visited-domains.json", "r+") as jf:
                data = json.load(jf)
                for js_line in data['sites_visited']:
                    try:
                        # make sure current page has not been visited before
                        # if it has skip adding it to matrix later
                        if cursor in js_line['url']:
                            stop_dupe = True
                        # if visited before don't add to que
                        if tmp in js_line['url']:
                            visited = True
                            break
                    except:
                        break
                    
            # check if already in que
            for j, elements in enumerate(que):
                if que[j] == tmp:
                    inQue = True
                    break
            if inQue:
                continue
            if visited:
                continue
            if tmp is None:
                continue
            if not rp.can_fetch("*", tmp):
                continue

            # if neither and it stays on site, add to que
            que.append(tmp)

        # write contents to file, increment page count
        # probobly a redundant write, leaving in for now
        with open(f'files/{pageName}.txt', 'w') as f:
            f.write(content)
            i = i + 1
            k = {
            "url": cursor,
            "ajacent": ajacent
            }
            # if page has been visited before don't re-add it to matrix
            if stop_dupe == False:
                with open('visited-domains.json', 'r+') as jf:
                    #print(k['url'])
                    data_js = json.load(jf)
                    data_js["sites_visited"].append(k)
                    jf.seek(0)
                    json.dump(data_js, jf, indent=4)
        #move to next item in que
        try:
            cursor = que.pop(0)
        except:
            print("no more links in que, see warning 1")
            return
        

if __name__ == '__main__':
    main()
