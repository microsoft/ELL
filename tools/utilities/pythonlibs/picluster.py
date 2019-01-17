####################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     picluster.py
#  Authors:  Chris Lovett
#
#  Requires: Python 3.x
#
####################################################################################################

import json
import os
import random
import re
import requests
import socket
import sys
import time
import uuid
import platform

import cpuinfo
import logger


class PiBoardEntity():
    def __init__(self, values=None):
        if isinstance(values, dict):
            self.load(values)
        else:
            self.ip_address = ""
            self.os_name = platform.platform()
            self.os_version = platform.version()
            self.platform = platform.uname()
            self.current_task_name = ""
            self.current_user_name = ""
            self.command = ""
            self.last_heartbeat = ""
            self.lock_key = ""
            self.comment = ""
            self.hostname = ""
            self.temperature = ""
            self.system_load = ""
            self.alive = False

    def load(self, values):
        self.ip_address = self.getValue(values, 'IpAddress')
        self.platform = self.getValue(values, 'Platform')
        self.os_name = self.getValue(values, 'OsName')
        self.os_version = self.getValue(values, 'OsVersion')
        self.current_task_name = self.getValue(values, 'CurrentTaskName')
        self.current_user_name = self.getValue(values, 'CurrentUserName')
        self.command = self.getValue(values, 'Command')
        self.last_heartbeat = self.getValue(values, 'LastHeartbeat')
        self.lock_key = self.getValue(values, 'LockKey')
        self.comment = self.getValue(values, 'Comment')
        self.hostname = self.getValue(values, 'HostName')
        self.temperature = self.getValue(values, 'Temperature')
        self.system_load = self.getValue(values, 'SystemLoad')
        self.alive = self.getValue(values, 'IsAlive')

    def getValue(self, d, name):
        if name in d:
            return d[name]
        return None

    def deserialize(self, text):
        values = json.loads(text)
        self.load(values)

    def serialize(self):
        self.os_name = platform.platform()
        self.os_version = platform.version()

        return json.dumps({'IpAddress': self.ip_address, "Platform": self.platform,
                           'OsName': self.os_name, 'OsVersion': self.os_version,
                           'CurrentTaskName': self.current_task_name, 'CurrentUserName': self.current_user_name,
                           'Command': self.command, "LockKey": self.lock_key, "Comment": self.comment,
                           'HostName': self.hostname, 'Temperature': self.temperature, 'SystemLoad': self.system_load,
                           'ApiKey': self.apikey})

    def matches(self, field_name, pattern):
        if pattern is None or pattern == []:
            return True
        value = getattr(self, field_name)
        if value is None:
            value = ""
        if isinstance(pattern, list):
            for p in pattern:
                m = re.match(p, value)
                # make sure pattern matches the entire value.
                if m and m.span(0)[1] == len(value):
                    return True
        else:
            m = re.match(pattern, value)
            if m and m.span(0)[1] == len(value):
                return True
        return False


class PiBoardTable:
    def __init__(self, endpoint, apikey, username=None):
        self.endpoint = endpoint
        self.username = username
        self.apikey = apikey
        self.ci = None
        self.logger = logger.get()
        if self.username is None:
            self.username = self.get_user_name()

    def send(self, entity, command):
        entity.apikey = self.apikey
        body = entity.serialize()
        headers = {'content-type': 'application/json'}
        r = requests.post(self.endpoint + command, data=body, headers=headers)
        if r.status_code != 200:
            raise Exception("update failed: " + str(r.status_code))
        e = json.loads(r.text)
        if isinstance(e, dict):
            return PiBoardEntity(e)
        return None

    def update(self, entity):
        # heartbeats from monitor.py should include platform info.
        if not self.ci:
            self.ci = cpuinfo.CpuInfo()
        entity.platform = self.ci.platform
        self.send(entity, "update")

    def get(self, id):
        r = requests.get("{}?id={}".format(self.endpoint, id))
        try:
            e = json.loads(r.text)
        except:
            self.logger.info("### Error parsing response: {}".format(r.text))
            sys.exit(1)

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
        e = self.get(id)
        if (e):
            return self.send(e, "delete")
        return None

    def lock(self, ip, jobName):
        # now try and lock the device for our usage.
        a = PiBoardEntity()
        a.ip_address = ip
        a.current_user_name = self.username
        a.command = "Lock"
        a.lock_key = str(uuid.uuid1())
        # make the job name unique in every instance
        a.current_task_name = jobName
        r = self.send(a, "lock")
        if r:
            if r.current_user_name != self.username:
                raise Exception("Lock failed on machine {}, already taken by {}".format(ip, r.current_user_name))
            elif r.command != "Lock":
                raise Exception("Lock failed on machine {}, lock not granted".format(ip))
            elif r.current_task_name != a.current_task_name:
                raise Exception("Lock failed on machine {}, already used by task {}".format(ip, r.current_task_name))
            elif r.lock_key != a.lock_key:
                raise Exception("Lock failed on machine {}, lock key mismatch {}".format(ip))
        else:
            raise Exception("Lock failed, machine {} not found".format(ip))
        return r

    def unlock(self, ip):
        # now try and lock the device for our usage.
        a = PiBoardEntity()
        a.ip_address = ip
        a.current_user_name = self.username
        a.command = "Free"
        r = self.send(a, "free")
        if r:
            if r.current_user_name != "" or r.command != "Free":
                raise Exception("Free failed")
        else:
            raise Exception("Free failed, machine {} not found".format(ip))
        return r

    def get_local_ip(self):
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        bing_ip = socket.gethostbyname('www.bing.com')
        s.connect((bing_ip, 80))
        localip, port = s.getsockname()
        s.close()
        return localip

    def get_user_name(self):
        name = os.getenv('USERNAME')
        if not name:
            name = os.getenv('USER')
        if name == "pi":
            name += "_" + self.get_local_ip()
        return name

    def wait_for_free_machine(self, jobName, reAddress=None, rePlatform=None):
        """ find a free machine on the cluster, optionally matching the given regex pattern on ip address or platform"""
        while True:
            self.logger.info("Waiting for a free machine")
            machines = self.get_matching_machines(reAddress, rePlatform)
            random.shuffle(machines)
            for e in machines:
                try:
                    self.logger.info("Trying to lock machine " + str(e.ip_address))
                    if e.alive and e.command != 'Lock':
                        result = self.lock(e.ip_address, jobName)
                        # no exception, so we got it
                        return result
                except:
                    pass

            self.logger.info("All machines are busy, sleeping 10 seconds and trying again...")
            time.sleep(10)

    def get_matching_machines(self, reAddress=None, rePlatform=None):
        machines = self.get_all()
        result = []
        for e in machines:
            platform_ok = e.matches("platform", rePlatform)
            address_ok = e.matches("ip_address", reAddress)
            if address_ok and platform_ok:
                result += [e]
        if result == [] and len(machines) > 0:
            self.logger.info("nothing matching ip_address '{}' and platform '{}'".format(reAddress, rePlatform))
        return result
