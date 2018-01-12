#!/usr/bin/env python3
####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     procmon.py
##  Authors:  Lisa Ong
##
##  Requires: Python 3.4+, psutil (pip install psutil)
##
####################################################################################################
import os
import sys
import argparse
import json
import psutil
import statistics # requires Python 3.4
from time import sleep

class ProcessMonitor:
    def __init__(self, process_id, output_file, interval):
        self.output_file = output_file
        self.interval = interval
        self.process = psutil.Process(process_id)

    def start(self):
        """While the process is running, monitor its vitals (e.g. resource usage)
           and log the results
        """
        stats = []
        try:
            firstcall = True
            while True:
                stat = self.process.as_dict(attrs=[
                    'cpu_times', 'cpu_percent', 'num_threads', 'memory_info'])
                stat['timestamp'] = psutil.boot_time()
                stat['system_cpu_percent'] = psutil.cpu_percent(interval=None, percpu=True)
                stat['system_cpu_freq'] = psutil.cpu_freq(percpu=True)

                if firstcall:
                    # first call is throwaway (see documentation for cpu_percent on why)
                    firstcall = False
                else:
                    stats.append(stat)

                sleep(self.interval)

        except psutil.NoSuchProcess:
            print("Process has exited")
        finally:
            summary = self.summarize(stats)

            results = {}
            results["stats"] = stats
            results["summary"] = summary

            if self.output_file:
                # write Windows line endings
                with open(self.output_file, 'w', encoding='utf-8', newline='\r\n') as outfile:
                    json.dump(results, outfile, ensure_ascii=False, indent=2, sort_keys=True)

    def summarize(self, stats):
        summary = {}

        # 'cpu_percent' is an aggregate across the logical CPUs in use
        summary["mean_cpu_percent"] = statistics.mean([x['cpu_percent'] for x in stats])
        summary["mean_num_threads"] = statistics.mean([x['num_threads'] for x in stats])
        summary["mean_system_cpu_percent"] = [
            statistics.mean([x['system_cpu_percent'][y] for x in stats])
                for y in range(0, psutil.cpu_count())
        ]

        summary["user_cpu_time_s"] = stats[-1]['cpu_times'].user
        summary["system_cpu_time_s"] = stats[-1]['cpu_times'].system

        # for best portablity, stick to subset of fields that are present in all OSes
        summary["mean_resident_set_b"] = statistics.mean([x['memory_info'].rss for x in stats])
        summary["mean_virtual_memory_b"] = statistics.mean([x['memory_info'].vms for x in stats])

        return summary

if __name__ == "__main__":
    parser = argparse.ArgumentParser()

    # required arguments
    parser.add_argument("process_id", type=int, help="process identifier to monitor")

    # options
    parser.add_argument("--interval", type=float, default=1, help="monitoring interval in seconds")
    parser.add_argument("--logfile", help="path to the output file")
    args = parser.parse_args()

    pm = ProcessMonitor(args.process_id, args.logfile, args.interval)
    pm.start()