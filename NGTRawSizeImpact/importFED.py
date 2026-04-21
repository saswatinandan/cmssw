from FEDlist import FEDlist
#import FWCore.ParameterSet.Config as cms
import numpy as np

def FEDinclude(dname):
    listofarray = FEDlist['BPIX+'] + FEDlist['BPIX-'] + FEDlist['FPIX+'] + FEDlist['FPIX-'] + FEDlist['EB+'] + FEDlist['EB-'] + FEDlist['EE+'] + FEDlist['EE-'] + FEDlist['ES+'] + FEDlist['ES-'] + FEDlist['HBHEA']+FEDlist['HBHEB']+FEDlist['HBHEC']+FEDlist['HF']+FEDlist['HO'] + FEDlist['TEC+']+FEDlist['TEC-']+FEDlist['TOB']+FEDlist['TIBTID'] + FEDlist['CSC+'] + FEDlist['CSC-']+FEDlist['DT+']+FEDlist['DT-']+FEDlist['DT0']+FEDlist['DTUP'] + FEDlist['RPC']
    if dname == 'Pixel':
        listofarray = FEDlist['BPIX+'] + FEDlist['BPIX-'] + FEDlist['FPIX+'] + FEDlist['FPIX-']
        
    elif dname == 'ECAL':
        listofarray =  FEDlist['EE-'] + FEDlist['EB+'] + FEDlist['EB-'] + FEDlist['EE+']
    elif dname == 'ES':
        listofarray = FEDlist['ES+'] + FEDlist['ES-']
    elif dname == 'HCAL':
        listofarray = FEDlist['HBHEB']+FEDlist['HBHEA']+FEDlist['HBHEC']+FEDlist['HF']+FEDlist['HO']
    elif dname == 'Strips':
        listofarray = FEDlist['TEC+']+FEDlist['TEC-']+FEDlist['TOB']+FEDlist['TIBTID']
    elif dname == 'Muons':
        listofarray = FEDlist['CSC+'] + FEDlist['CSC-']+FEDlist['DT+']+FEDlist['DT-']+FEDlist['DT0']+FEDlist['DTUP'] + FEDlist['RPC'] + FEDlist['GEM+'] + FEDlist['GEM-']+ FEDlist['GEMPILOT'] 
    #elif dname == 'All':
    else:
        listofarray = FEDlist['CALOL1']+FEDlist['CALOL2']+FEDlist['CPM-PRI']+FEDlist['TWINMUX']+FEDlist['TOTDET']
    arr = [list(ar) if type(ar) == range else [ar] for ar in listofarray]
    n_arr = arr[0]
    for i in range(1,len(arr)):
        n_arr = n_arr+arr[i]
        
    return n_arr


def FEDexclude(dname):
    detector_names = ['BPIX+','BPIX-','FPIX+','FPIX-','EB+','EB-','EE+','EE-','ES+','ES-','HBHEA','HBHEB','HBHEC','HF','HO','TEC+','TEC-','TOB','TIBTID','CSC+','CSC-','DT+','DT-','DT0','DTUP','RPC','GEM+','GEM-','GEMPILOT','CALOL1','CALOL2','MUTF','UGT','UGTSPARE','CTPPS','TOTDET','CPM-PRI','TWINMUX']
    listofarray = []
    if dname == 'Pixel':
        pixelonly = [listofarray+FEDlist[name] for name in detector_names if name not in ['BPIX+','BPIX-','FPIX+','FPIX-']]
        listofarray = pixelonly
    elif dname == 'Strips':
        stripsonly = [listofarray+FEDlist[name] for name in detector_names if name not in ['TEC+','TEC-','TOB','TIBTID']]
        listofarray = stripsonly
    elif dname == 'ECAL':
        ecalonly = [listofarray+FEDlist[name] for name in detector_names if name not in ['EB+','EB-','EE+','EE-','ES+','ES-']]
        listofarray = ecalonly
    elif dname == 'HCAL':
        hcalonly = [listofarray+FEDlist[name] for name in detector_names if name not in ['HBHEA','HBHEB','HBHEC','HF','HO']]
        listofarray = hcalonly
    elif dname == 'Muons':
        muononly = [listofarray+FEDlist[name] for name in detector_names if name not in ['CSC+','CSC-','DT+','DT-','DT0','DTUP','RPC','GEM+','GEM-','GEMPILOT']]
        listofarray = muononly
    elif dname == 'All':
        All = [listofarray+FEDlist[name] for name in detector_names]
        listofarray = All
    elif dname == 'L1':
        L1only = [listofarray+FEDlist[name] for name in detector_names if name not in ['CALOL1','CALOL2','MUTF','UGT','UGTSPARE']]
        listofarray = L1only
    else:
        others = [listofarray+FEDlist[name] for name in detector_names if name not in ['CALOL1','CALOL2','MUTF','UGT','UGTSPARE','CTPPS','CPM-PRI','TWINMUX']]
        listofarray = others
        
    arr = [list(ar[0]) if type(ar[0]) == range else [ar[0]] for ar in listofarray]
    n_arr = arr[0]
    for i in range(1,len(arr)):
        n_arr = n_arr+arr[i]        
    return n_arr

#print(FEDexclude('All'))

    
