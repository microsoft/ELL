"""Helper functions to download files and subsequently extract model zip files"""

import os
import requests
import ziptools


def download_file(url):
    """Downloads file pointed to by `url`."""
    filename = os.path.basename(url)
    # Download the file
    response = requests.get(url, stream=True)

    # Write the file to disk
    with open(filename, "wb") as handle:
        handle.write(response.content)
    return filename


def download_and_extract_model(url, model_extension='.cntk'):
    """Downloads file pointed to by `url`. Once downloaded, unzips the
    downloaded file.
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
    filename = download_file(url)

    # Extract the file if it's a zip
    unzip = ziptools.Extractor(filename)
    success, model_filename = unzip.extract_file(model_extension)
    if success:
        print("extracted zipped model: " + model_filename)
    else:
        print("non-zipped model: " + filename)
        model_filename = filename

    return model_name

