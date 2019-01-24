####################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     logger.py
#  Authors:  Chris Lovett, Lisa Ong
#
#  Requires: Python 3.x
#
####################################################################################################

import logging


def add_logging_args(arg_parser):
    verbosity = arg_parser.add_mutually_exclusive_group()
    verbosity.add_argument("--verbosity", help="Turn on verbosity logging level. Available levels are {}".format(
        list(logging._levelToName.values())), choices=list(logging._levelToName.values()), metavar="<LEVEL>",
        default="INFO")
    verbosity.add_argument("--silence", help="Turn off logging.", action="store_true", default=False)
    arg_parser.add_argument("--logfile", help="Store output to log file.", metavar="<LOG_FILE>", default=None)
    arg_parser.add_argument("--logmode", help="Set log mode, 'a' or 'w' for append or not append.", default='w')


class Log:
    def __init__(self, verbosity, logfile, logmode, format):
        logging.basicConfig(level=verbosity, format=format)
        self.verbosity = verbosity
        self.silent = verbosity == 0
        self.logger = logging.getLogger(__name__)
        self.logger.setLevel(self.verbosity)
        self.logger_console_handler = None
        if logfile:
            self.logger_file_handler = logging.FileHandler(logfile, mode=logmode)
            self.logger_file_handler.setFormatter(logging.Formatter(format))
            self.logger_file_handler.setLevel(self.verbosity)
            self.logger.addHandler(self.logger_file_handler)
        else:
            self.logger_file_handler = None

    def __reduce__(self):
        return (Log, (self.verbosity, "", 'a'))

    def __exit__(self, type, value, traceback):
        if self.logger_file_handler:
            self.logger.removeHandler(self.logger_file_handler)
        if self.logger_console_handler:
            self.logger.removeHandler(self.logger_console_handler)

    def debug(self, msg, *args, **kwargs):
        if not self.silent:
            self.logger.debug(msg, *args, **kwargs)

    def info(self, msg, *args, **kwargs):
        if not self.silent:
            self.logger.info(msg, *args, **kwargs)

    def verbose(self, msg, *args, **kwargs):
        if not self.silent:
            self.logger.info(msg, *args, **kwargs)

    def warning(self, msg, *args, **kwargs):
        if not self.silent:
            self.logger.warning(msg, *args, **kwargs)

    def error(self, msg, *args, **kwargs):
        if not self.silent:
            self.logger.error(msg, *args, **kwargs)

    def getVerbosity(self):
        return logging.getLevelName(self.logger.getEffectiveLevel())

    def getVerbose(self):
        return self.logger.getEffectiveLevel() <= logging.INFO and not self.getSilent()

    def getEffectiveLevel(self):
        return self.logger.getEffectiveLevel()

    def getSilent(self):
        return self.silent

    def exception(self, error_info):
        errorType, value, stack = error_info
        import traceback
        self.error("### Exception: {}: {}".format(errorType, value))
        for line in traceback.format_tb(stack):
            self.error(line.strip())

    def critical(self, msg, *args, **kwargs):
        self.logger.critical(msg, *args, **kwargs)

    fatal = critical


_logger = None


def initialized():
    return _logger is not None


def init(verbosity="INFO", logfile=None, logmode='w', format="%(message)s"):
    global _logger
    _logger = Log(verbosity, logfile, logmode, format)
    return _logger


def setup(args=None, format="%(message)s"):
    """
    setup the logger given parsed command line args and a message format pattern.
    The default pattern is "%(message)s" but more interesting patterns like this can
    help with multithreaded output: "%(asctime)s [%(thread)d]: %(message)s".
    """
    global _logger
    verbosity = "INFO"
    logmode = 'w'
    logfile = None
    if args:
        verbosity = args.verbosity
        logfile = args.logfile
        logmode = args.logmode
        if args.silence:
            verbosity = 0

    return init(verbosity, logfile, logmode, format)


def get():
    global _logger
    if _logger is None:
        raise Exception("logger.setup(...) has not been called, please fix your main entry point")
    return _logger
