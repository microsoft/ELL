"""Helper functions to download files and subsequently extract model zip files"""

import os
import requests
import ziptools
import logging

_logger = logging.getLogger(__name__)

def download_file(url, local_folder=None):
    """Downloads file pointed to by `url`.
    If `local_folder` is not supplied, downloads to the current folder.
    """
    filename = os.path.basename(url)
    if local_folder:
        filename = os.path.join(local_folder, filename)

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

    # Extract the file if it's a zip
    unzip = ziptools.Extractor(filename)
    success, model_filename = unzip.extract_file(model_extension)
    if success:
        _logger.info("Extracted zipped model: " + model_filename)
    else:
        _logger.info("Non-zipped model: " + filename)

    return model_name
