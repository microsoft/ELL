'Class that lists the model files available on Github and provides a way to choose one interactively'

import re
from collections import defaultdict
from github import Github
from ipywidgets import interact
import ipywidgets as widgets
from ..pretrained_model import PretrainedModel

class ModelChooser:
    'Choose models from Github'

    def __init__(self):
        self.model = None
        repo = Github().get_organization('Microsoft').get_repo('ELL-models')
        model_dirs = repo.get_contents('models/ILSVRC2012')
        self.model_files = [
            d.name for d in model_dirs if d.name.lower().find('labels') == -1
        ]
        self.modelre = re.compile(
            '(?P<src>[a-z])_[A-Z](?P<size>\d+x\d+x\d+)(?P<arch>([A-Z]\d*)+)')
        self.grouped = defaultdict(list)
        for desc in [self.parse_name(n) for n in self.model_files]:
            self.grouped[desc['size']].append(desc)

    def parse_name(self, model_name):
        'Parse the succinct naming convention used for model files'
        match = self.modelre.match(model_name)
        return {
            'modelarch': match.group('src'),
            'size': match.group('size'),
            'layers': match.group('arch'),
            'model_name': model_name
        }

    def choose_model(self):
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
