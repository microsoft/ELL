import os
import os.path
import requests
import shutil

class Downloader:
    def __init__(self, model_dir):
        self.model_dir = model_dir
        if not os.path.exists(model_dir):
            os.makedirs(model_dir)
        
    def _download(self, url, filename, download_once, binary):
        path = os.path.join(self.model_dir, filename)
        # get the file unless we've gotten it before
        if not download_once or not os.path.exists(path):
            flag = 'wb' if binary else 'w'
            with open(path, flag) as f:
                import requests
                resp = requests.get(url, stream=binary)
                if resp.status_code != 200:
                    raise 'cannot download ' + url
                if binary:
                    resp.raw.decode_content = True
                    shutil.copyfileobj(resp.raw, f)
                else:
                    f.write(resp.text)
        return path
        
    def download(self, url, filename, download_once = True):
        return self._download(url, filename, download_once, False)

    def download_binary(self, url, filename, download_once = True):
        return self._download(url, filename, download_once, True)
