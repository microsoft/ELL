import os
from urllib import request

class Downloader:
    def __init__(self):
        return
        
    def download(self, url, local_path):
        req = request.URLopener()
        req.retrieve(url, local_path)
