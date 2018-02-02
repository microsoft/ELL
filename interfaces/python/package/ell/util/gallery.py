'Class that lists the model files available on Github and provides a way to choose one interactively'

import re
from collections import defaultdict
import os
import time
import datetime

class ModelGallery:
    'Choose models from the Model Gallery on Github'

    def __init__(self):
        self.model = None
        self.cache_file = 'gallery.json'

        download = True
        if os.path.isfile(self.cache_file):
            diff = time.time() - datetime.datetime.fromtimestamp(os.stat(self.cache_file).st_mtime).timestamp()
            if (diff < 3600):
                download = False # use our cache then.
        
        if download:
            from github import Github
            repo = Github().get_organization('Microsoft').get_repo('ELL-models')
            model_dirs = repo.get_contents('models/ILSVRC2012')

            modelre = re.compile(
                '(?P<src>[a-z])_[A-Z](?P<size>\d+x\d+x\d+)(?P<arch>([A-Z]\d*)+)')
            descriptions = [{
                'modelarch': match.group('src'),
                'size': match.group('size'),
                'layers': match.group('arch'),
                'model_name': match.string
            } for match in [modelre.match(d.name) for d in model_dirs] if match]        
            self.save_list(descriptions)
        else:
            descriptions = self.load_list()

        self.grouped = defaultdict(list)
        for desc in descriptions:
            self.grouped[desc['size']].append(desc)

    def save_list(self, data):
        import json 
        with open(self.cache_file, 'w') as outfile:
            json.dump(data, outfile)

    def load_list(self):
        import json 
        with open(self.cache_file) as f:
            return json.loads(f.read())

    def choose_model(self):
        from ipywidgets import interact
        import ipywidgets as widgets
        from .pretrained_model import PretrainedModel

        'Provide an interactive way to choose between models and call a callback when a choice is made'
        keys = list(self.grouped.keys())
        size_dropdown = widgets.Dropdown(options=keys, description='Size')
        arch_dropdown = widgets.Dropdown(
            options=[d['layers'] for d in self.grouped[keys[0]]],
            description='Layers')

        def update_arch(*args):
            'update the architecture choices for the latest size choice'
            idx = size_dropdown.options.index(size_dropdown.value)
            arch_dropdown.options = [
                d['layers'] for d in self.grouped[keys[idx]]
            ]

        size_dropdown.observe(update_arch, 'value')

        def choose(size, layers):
            'call the callback when the user makes interactive choices'
            try:
                idx = [d['layers'] for d in self.grouped[size]].index(layers)
            except ValueError:
                # size updated, so layers will not be found
                idx = 0
            model_name = self.grouped[size][idx]['model_name']
            self.model = PretrainedModel(model_name)

        interact(choose, size=size_dropdown, layers=arch_dropdown)
