"""Helper functions to download files and subsequently extract model zip files"""

import os
import requests
import subprocess
import ziptools
import logging
from shutil import copyfile, rmtree

_logger = logging.getLogger(__name__)
script_dir = os.path.dirname(os.path.abspath(__file__))

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
    """Downloads file pointed to by `url`. Once downloaded, unzips the
    downloaded file.
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


def run(args):
    """ Run a command """
    proc = subprocess.Popen(
        args,
        stdout=subprocess.PIPE, stderr=subprocess.PIPE,
        bufsize=0, universal_newlines=True)
    try:
        outs, errors = proc.communicate()
    except subprocess.TimeoutExpired:
        proc.kill()
        outs, errors = proc.communicate()

    print(outs)
    print(errors)

    if proc.returncode != 0:
        raise Exception("Command failed: {}".format(" ".join(args)))

def clone_repo(url):
    """ Clone the given git repo into the user's HOME directory """
    home = os.getenv("HOME")
    if os.name == 'nt':
        home = os.getenv("USERPROFILE")
        if "NetworkService" in home:
            home = os.path.join(os.path.splitdrive(script_dir)[0] + os.path.sep,"VSTS","agent")

    if not os.path.isdir(home):
        raise Exception("Cannot find user's HOME directory: {}".format(home))

    repo_name = os.path.basename(url)

    saved = os.getcwd()
    repo = os.path.join(home, repo_name)

    if os.path.isdir(repo):
        print("### Updating git repo: '{}' at '{}'".format(repo_name, home))
        os.chdir(repo)
        run(["git", "pull"])
    else:
        os.chdir(home)
        print("### Cloning git repo: '{}' into '{}'".format(repo_name, home))
        run(["git", "lfs", "install"])
        run(["git", "clone", url])

    os.chdir(saved)
    return repo + os.path.sep
    
