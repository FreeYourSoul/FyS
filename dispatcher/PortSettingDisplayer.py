import configparser
import os

for root, directories, fileNames in os.walk('./'):
    for filename in fileNames:
        if ".ini" in filename:
            config = configparser.ConfigParser()
            path = os.path.join(root, filename)
            config.read(path)
            print("Configuration file {}".format(path))
            print("Port Proxy FrontEnd: {} Port Proxy backend: {}".format(config["proxy_frontend"]["port"], config["proxy_backend"]["port"]))
