####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     logger.py
##  Authors:  Chris Lovett, Lisa Ong
##
##  Requires: Python 3.x
##
####################################################################################################

import logging

def get(filepath=None, log_thread_id=False):
    """Returns the global logger
    Logs to a file if filepath is provided
    """

    if log_thread_id:
        log_format = "%(asctime)s [%(thread)d]: %(message)s"
    else:
        log_format = "%(asctime)s %(message)s"
        
    logging.basicConfig(level=logging.INFO, format=log_format)
    
    logger = logging.getLogger(__name__)

    if filepath:
        handler = logging.FileHandler(filepath)
        handler.setFormatter(logging.Formatter(log_format))
        logger.addHandler(handler) 

    return logger
