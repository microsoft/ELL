####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     download_helper.py
##  Authors:  Chris Lovett
##
##  Requires: Python 3.x
##
####################################################################################################

"""
Helper functions to download files and subsequently extract model zip files.
Also provides helper for cloning a git repo and finding the files there instead.
"""

import os
import requests
import subprocess
import ziptools
import logging
from shutil import copyfile, rmtree

_logger = logging.getLogger(__name__)
script_dir = os.path.dirname(os.path.abspath(__file__))
_cloned_repos = {}

def download_file(url, local_folder=None):
    """Downloads file pointed to by `url`.
    If `local_folder` is not supplied, downloads to the current folder.
    """
    filename = os.path.basename(url)
    if local_folder:
        filename = os.path.join(local_folder, filename)

    if os.path.isfile(url):
        _logger.info("Using cached local file: {} ({} bytes)".format(url, os.path.getsize(url)))
        copyfile(url, filename)
        return filename

    # Download the file
    _logger.info("Downloading: " + url)
    response = requests.get(url, stream=True)
    if response.status_code != 200:
        raise Exception("download file failed with status code: %d, fetching url '%s'" % (response.status_code, url))

    # Write the file to disk
    with open(filename, "wb") as handle:
        handle.write(response.content)
    return filename


def download_and_extract_model(url, model_extension='.cntk', local_folder=None):
    """Downloads file pointed to by `url`. Once downloaded, unzips the downloaded file.
    If `local_folder` is not supplied, downloads to the current folder.
    """
    def get_model_name(url):
        filename = os.path.basename(url)
        model_filename, ext = os.path.splitext(filename)
        if (not ext == '.zip'):
            model_filename = filename
        model_name, _ = os.path.splitext(model_filename)
        return model_name

    model_name = get_model_name(url)

    # Download the file
    filename = download_file(url, local_folder)
    _logger.info("Extracting zipped model: " + filename)

    # Extract the file if it's a zip
    unzip = ziptools.Extractor(filename)
    success, model_filename = unzip.extract_file(model_extension)
    if success:
        _logger.info("Extracted zipped model: " + model_filename)
    else:
        _logger.info("Non-zipped model: " + filename)

    return model_name


def _run(args):
    """ Run a subprocess specified by args """
    proc = subprocess.Popen(
        args,
        stdout=subprocess.PIPE, stderr=subprocess.PIPE,
        bufsize=0, universal_newlines=True)
    try:
        outs, errors = proc.communicate()
    except subprocess.TimeoutExpired:
        proc.kill()
        outs, errors = proc.communicate()

    _logger.info(outs)
    if len(errors) > 0:
        _logger.error(errors)

    if proc.returncode != 0:
        raise Exception("Command failed: {}".format(" ".join(args)))

def get_home_path():
    """ get location of the current user's home path """
    home = os.getenv("HOME")
    if os.name == 'nt':
        home = os.getenv("USERPROFILE")
        if "NetworkService" in home:
            home = os.path.join(os.path.splitdrive(script_dir)[0] + os.path.sep, "git")
            if not os.path.isdir(home):
                os.mkdir(home)

    if not os.path.isdir(home):
        raise Exception("Cannot find user's HOME directory: {}".format(home))

    return home

def clone_repo(url, target_directory):    
    """ Clone the given git repo into the target_directory """
    global _cloned_repos
    
    repo_name = os.path.basename(url)

    saved = os.getcwd()
    repo = os.path.join(target_directory, repo_name)

    if url in _cloned_repos:
        return _cloned_repos[url]

    if os.path.isdir(repo):
        _logger.info("### Updating git repo: '{}' at '{}'".format(repo_name, target_directory))
        os.chdir(repo)
        _run(["git", "pull"])
    else:
        os.chdir(target_directory)
        _logger.info("### Cloning git repo: '{}' into '{}'".format(repo_name, target_directory))
        _run(["git", "lfs", "install"])
        _run(["git", "clone", url])

    os.chdir(saved)

    result = repo + os.path.sep
    _cloned_repos[url] = result

    return result
    
