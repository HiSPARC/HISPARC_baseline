import zlib
import tables
import numpy as np

DATAPATH = '../2014_5_31.h5'

def get_traces_baseline(station_group='/hisparc/cluster_amsterdam/station_501'):
    with tables.open_file(DATAPATH, 'r') as data:
        station = data.get_node(station_group)
        events = station.events
        blobs = station.blobs
        for event in events:
			traces = get_traces_for_event(blobs, event)
			baselines = [b for b in event['baseline'] if b not in [-1, -999]]
			n_peaks = [p for p in event['n_peaks'] if p not in [-1, -999]]
			pulseheights = [h for h in event['pulseheights'] if not h == -999]
			integrals = [i for i in event['integrals'] if not i == -999]
			timestamp = [event['ext_timestamp'] for i in range(4)]
			yield traces, baselines, n_peaks, pulseheights, integrals, timestamp

def get_traces_for_event(blobs, event):
	trace_ids = [trace_id for trace_id in event['traces'] if not trace_id == -1]
	traces = [get_trace(blobs, id) for id in trace_ids]
	return traces

def get_trace(blobs, id):
	return [int(x) for x in zlib.decompress(blobs[id]).split(',') if not x == '']

	
if __name__ == '__main__':
    get_trace_baseline(1, 1)
