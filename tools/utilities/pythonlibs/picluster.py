####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     picluster.py
##  Authors:  Chris Lovett
##
##  Requires: Python 3.x
##
####################################################################################################

import datetime
from dateutil.parser import parse
import json
import os
import random
import requests
import socket
import time
import uuid

import logger

class PiBoardEntity():
    def __init__(self, values = None):
        if isinstance(values, dict):
            self.load(values)
        else:
            self.ip_address = ""
            self.os_name = ""
            self.os_version = ""
            self.current_task_name = ""
            self.current_user_name = ""
            self.command = ""
            self.last_heartbeat = ""
            self.lock_key = ""

    def load(self, values) :
        self.ip_address = values['IpAddress']
        self.os_name = values['OsName']
        self.os_version = values['OsVersion']
        self.current_task_name = values['CurrentTaskName']
        self.current_user_name = values['CurrentUserName']
        self.command = values['Command']
        self.last_heartbeat = values['LastHeartbeat']
        self.lock_key = values['LockKey']

    def deserialize(self, text):
        values = json.loads(text)
        self.load(values)

    def serialize(self):
        return json.dumps({'IpAddress': self.ip_address,
                            'OsName': self.os_name, 'OsVersion': self.os_version,
                            'CurrentTaskName': self.current_task_name, 'CurrentUserName': self.current_user_name,
                            'Command':self.command, "LockKey": self.lock_key })

class PiBoardTable:
    def __init__(self, endpoint, username = None):
        self.logger = logger.get()
        self.endpoint = endpoint
        self.username = username
        if self.username is None:
            self.username = self.get_user_name()

    def update(self, entity):
        body = entity.serialize()
        headers = {'content-type': 'application/json'}
        r = requests.post(self.endpoint, data=body, headers=headers)
        if r.status_code != 200:
            raise Exception("update failed: " + str(r.status_code))
        e = json.loads(r.text)
        if isinstance(e, dict):
            return PiBoardEntity(e)
        return None

    def get(self, id):
        r = requests.get(self.endpoint + "?id=" + id)
        e = json.loads(r.text)
        if len(e) > 0:
            e = e[0]
            if isinstance(e, dict):
                return PiBoardEntity(e)
        return None

    def get_all(self):
        r = requests.get(self.endpoint)
        result = []
        for i in json.loads(r.text):
            result.append(PiBoardEntity(i))
        return result

    def delete(self, id):
        return requests.delete(self.endpoint + "?id=" + id)

    def lock(self, ip, jobName):
        # now try and lock the device for our usage.
        a = PiBoardEntity()
        a.ip_address = ip
        a.current_user_name = self.username
        a.command = "Lock"
        a.lock_key = str(uuid.uuid1())
        # make the job name unique in every instance
        a.current_task_name = jobName
        r = self.update(a)
        if r.current_user_name != self.username:
            raise Exception("Lock failed on machine {}, already taken by {}".format(ip, r.current_user_name))
        elif r.command != "Lock":
            raise Exception("Lock failed on machine {}, lock not granted".format(ip))
        elif r.current_task_name != a.current_task_name:
            raise Exception("Lock failed on machine {}, already used by task {}".format(ip, r.current_task_name))
        elif r.lock_key != a.lock_key:
            raise Exception("Lock failed on machine {}, lock key mismatch {}".format(ip))
        return r

    def unlock(self, ip):
        # now try and lock the device for our usage.
        a = PiBoardEntity()
        a.ip_address = ip
        a.current_user_name = self.username
        a.command = "Free"
        r = self.update(a)
        if r.current_user_name != "" or r.command != "Free":
            raise Exception("Free failed")
        return r

    def get_local_ip(self):
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        bing_ip = socket.gethostbyname('www.bing.com')
        s.connect((bing_ip , 80))
        localip, port = s.getsockname()
        s.close()
        return localip

    def get_user_name(self):
        name = os.getenv('USERNAME')
        if name == "":
            name = os.getenv('USER')
        if name == "pi":
            name += "_" + self.get_local_ip()
        return name

    def heartbeat_is_valid(self, heartbeat):
        # heartbeat should be updating every minute, we'll give it max of 5 minutes to comply
        now = datetime.datetime.utcnow()
        h = datetime.datetime.utcfromtimestamp(heartbeat.timestamp())
        diff = abs(now.timestamp() - h.timestamp())
        return diff < 5 * 60

    def wait_for_free_machine(self, jobName):
        # then this is a pi cluster server, so find a free machine
        while True:
            machines = self.get_all()
            random.shuffle(machines)
            for e in machines:
                try:
                    heartbeat = parse(e.last_heartbeat)
                    if not self.heartbeat_is_valid(heartbeat):
                        self.logger.info("note: machine at %s is not sending heartbeats" % (e.ip_address))
                    elif e.command != 'Lock':
                        result = self.lock(e.ip_address, jobName)
                        # no exception, so we got it
                        return result
                except:
                    pass

            self.logger.info("All machines are busy, sleeping 10 seconds and trying again...")
            time.sleep(10)
