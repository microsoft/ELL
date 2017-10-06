import os
import requests
import json
import socket
import time

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

    def load(self, values) :        
        self.ip_address = values['IpAddress']
        self.os_name = values['OsName']
        self.os_version = values['OsVersion']
        self.current_task_name = values['CurrentTaskName']
        self.current_user_name = values['CurrentUserName']
        self.command = values['Command']

    def deserialize(self, text):
        values = json.loads(text)
        self.load(values)

    def serialize(self):
        return json.dumps({'IpAddress': self.ip_address, 
                            'OsName': self.os_name, 'OsVersion': self.os_version, 
                            'CurrentTaskName': self.current_task_name, 'CurrentUserName': self.current_user_name,
                            'Command':self.command })


class PiBoardTable:
    def __init__(self, endpoint, username = None):
        self.endpoint = endpoint
        self.username = username
        if self.username is None:
            self.username = self.get_user_name()

    def update(self, entity):
        body = entity.serialize()
        headers = {'content-type': 'application/json'}
        r = requests.post(self.endpoint, data=body, headers=headers)
        print("update result: " + str(r))
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
        a.current_task_name = jobName
        r = self.update(a)
        if r.current_user_name != self.username or r.command != "Lock":
            raise Exception("Lock failed")
        return r
    
    def free(self, ip):
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

    def wait_for_free_machine(self, jobName):
        # then this is a pi cluster server, so find a free machine
        while True:
            for e in self.get_all():
                if e.command != 'Lock':
                    print(self.username + " is attempting to lock machine at " + e.ip_address)
                    result = self.lock(e.ip_address, jobName)
                    if result.current_user_name == self.username:
                        # we got it
                        return result
            print("All machines are busy, sleeping 10 seconds and trying again...")
            time.sleep(10)
