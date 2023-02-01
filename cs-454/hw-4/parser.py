#!/usr/bin/python3

from crawler import crawler
from whoosh.fields import Schema, STORED, ID, KEYWORD, TEXT
import whoosh as ws
from whoosh.index import create_in
import os
import time

class parser:
    def __init__(self):
        pass

    def create_schema(self):
        self.schema = ws.fields.Schema(title=TEXT(stored=True), content=TEXT,
            path=ID(stored=True), tags=KEYWORD, icon=STORED)

    def create_index(self):
        if not os.path.exists("index"):
            os.mkdir("index")
        self.ix = ws.index.create_in("index",self.schema)


    def load_index(self):
        pass

        
    def add_to_index(self, title, content, path, tags=None):
        writer = self.ix.writer()
        writer.add_document(title=title, content=content, path=path, tags=tags)
        writer.commit()

    def add_dir_to_index(self, fdir):
        if not os.path.exists(fdir):
            print("ERROR: dir: "+fdir+" not found")
            return
        if not os.path.exists("index"):
            self.create_index()
        ix = ws.index.open_dir("index")
        writer = ix.writer()
        file_list = os.listdir(fdir)
        for i in range(len(file_list)):
            with open(("files/"+file_list[i]), 'r') as file:
                name = file_list[i]
                c_time = time.ctime(os.path.getctime("files/"+name))
                tags = None
                icon = None
                contents = file.read()
                writer.add_document(title=name, content=contents)
        writer.commit()



    # @param {string} query to search for
    # @param {int} which page to display (1 page is 10 results)
    def search(self, query, page=1, disjunctive=False):
        with self.ix.searcher() as searcher:
            if disjunctive == True:
                parser = ws.qparser.QueryParser("content", self.ix.schema, group=ws.qparser.OrGroup)
            else:
                parser = ws.qparser.QueryParser("content", self.ix.schema)
            query = parser.parse(query)
            result = searcher.search(query)
            print("Showing results "+str(page-1)*10+" - "+str(page*10)+" out of "+str(len(results)))
            for i in range((page-1)*10,page*10):
                print(result[i])
