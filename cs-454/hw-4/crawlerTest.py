#!/usr/bin/python3

from crawler import crawler


c = crawler('https://en.wikipedia.org/wiki/Main_Page','https://en.wikipedia.org/')
c.crawl(10000, verbose=True)
'''
c = crawler('https://www.usnews.com','https://www.usnews.com')
c.crawl(100,verbose=True)
'''