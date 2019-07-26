import json
import os


class ModelOptions:
    def __init__(self):
        self.architecture = "GRU"
        self.num_layers = 2
        self.hidden_units = 128
        self.filename = ""
        self.wRank = None
        self.uRank = None
        self.gate_nonlinearity = "sigmoid"
        self.update_nonlinearity = "tanh"


class DatasetOptions:
    def __init__(self):
        self.name = "speechcommandsv01"
        self.featurizer = "featurizer_mel_16000_512_512_80_40_log"
        self.categories = "categories.txt"
        self.path = ""
        self.auto_scale = True
        self.normalize = False


class OptimizerOptions:
    def __init__(self):
        self.weight_decay = 1e-5
        self.momentum = 0  # RMSprop
        self.centered = False  # RMSprop
        self.alpha = 0  # ASGD, RMSprop
        self.eps = 1e-8
        self.rho = 0  # Adadelta
        self.lr_decay = 0  # Adagrad
        self.betas = (0.9, 0.999)  # Adam, SparseAdam, Adamax
        self.lambd = 0.0001  # ASGD
        self.t0 = 1000000.0  # ASGD
        self.etas = (0.5, 1.2)  # Rprop
        self.dampening = 0  # SGD
        self.step_sizes = (1e-06, 50)  # Rprop


class TrainingOptions:
    def __init__(self):
        self.max_epochs = 30
        self.learning_rate = 1e-3
        self.lr_scheduler = None
        self.lr_peaks = 1
        self.lr_min = 1e-5
        self.lr_gamma = 1
        self.lr_step_size = 1
        self.batch_size = 128
        self.optimizer = "RMSprop"
        self.optimizer_options = OptimizerOptions()
        self.use_gpu = True
        self.rolling = False
        self.max_rolling_length = 100


class TrainingConfig:
    def __init__(self):
        self.name = ""
        self.description = ""
        self.folder = None

        self.model = ModelOptions()
        self.dataset = DatasetOptions()
        self.training = TrainingOptions()

        self.job_id = None
        self.status = None
        self.downloaded = False
        self.last_modified = 0
        self.retries = 0
        self.filename = None
        self.sweep = None

    def set(self, name, value):
        if name not in self.__dict__:
            self.__dict__[name] = value
        else:
            t = self.__dict__[name]
            if type(t) == int:
                self.__dict__[name] = int(value)
            if type(t) == float:
                self.__dict__[name] = float(value)
            if type(t) == str:
                self.__dict__[name] = str(value)
            else:
                self.__dict__[name] = value

    def load(self, filename):
        with open(filename, "r") as f:
            data = json.load(f)
        TrainingConfig.from_dict(self, data)
        self.filename = filename
        self.last_modified = os.path.getmtime(self.filename)

    def save(self, filename):
        """ save an options.json file in the self.filename location """
        self.filename = filename
        data = TrainingConfig.to_dict(self)
        data["model"] = self.model.__dict__
        with open(filename, "w") as f:
            json.dump(data, f, indent=2, sort_keys=True)
        self.last_modified = os.path.getmtime(self.filename)

    @staticmethod
    def to_dict(obj):
        data = dict(obj.__dict__)
        for k in data:
            o = data[k]
            if hasattr(o, "__dict__"):
                data[k] = TrainingConfig.to_dict(o)
        return data

    @staticmethod
    def from_dict(obj, data):
        for k in data:
            v = data[k]
            if not hasattr(obj, k):
                setattr(obj, k, v)
            else:
                if isinstance(v, dict):
                    TrainingConfig.from_dict(getattr(obj, k), v)
                elif isinstance(getattr(obj, k), tuple):
                    setattr(obj, k, tuple(v))
                else:
                    setattr(obj, k, v)
