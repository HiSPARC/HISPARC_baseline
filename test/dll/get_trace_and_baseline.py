import zlib
import tables
import numpy as np

DATAPATH = '2014_5_31.h5'

def get_traces_baseline(station_group='/hisparc/cluster_amsterdam/station_501'):
    with tables.open_file(DATAPATH, 'r') as data:
        station = data.get_node(station_group)
        events = station.events
        blobs = station.blobs
        for event in events:
			traces = get_traces_for_event(blobs, event)
			baselines = [b for b in event['baseline'] if not b == -1]
			std_devs = [s for s in event['std_dev'] if not s == -1]
			yield traces, baselines, std_devs

def get_traces_for_event(blobs, event):
	trace_ids = [trace_id for trace_id in event['traces'] if not trace_id == -1]
	traces = [get_trace(blobs, id) for id in trace_ids]
	return traces

def get_trace(blobs, id):
	return [int(x) for x in zlib.decompress(blobs[id]).split(',') if not x == '']

	
if __name__ == '__main__':
    get_trace_baseline(1, 1)
