## Installation
```sh
  cmsrel CMSSW_14_0_11
  cd CMSSW_14_0_11/src/
  cmsenv
  git cms-init
  git pull git@github.com:saswatinandan/cmssw.git saswati
  git cms-addpkg Configuration/Eras DataFormats/SiStripCluster
  git cms-addpkg RecoLocalTracker/SiStripClusterizer
  scram b -j 8
```
## voms proxy
 ```sh
    voms-proxy-init -voms cms -valid 200:0
```
## Command to add track information for raw
```sh
cmsDriver.py step5 --conditions auto:run3_data_prompt -s RAW2DIGI,L1Reco,RECO --datatier RECO --eventcontent RECO --data --process reRECO --scenario pp -n 100 --repacked --era Run3_2024 --nThreads 254 --filein /store/data/Run2024F/Muon0/RAW-RECO/ZMu-PromptReco-v1/000/382/216/00000/aadd1ab9-4eb8-4fb2-ac62-bdd1bebe882e.root
cmsRun sep19_2_2_dump_raw.py inputFiles=step5_RAW2DIGI_L1Reco_RECO.root outputFile=flatntuple_step5_RAW2DIGI_L1Reco_RECO.root c=1
```
One should run the above command first as the output of the previous command will be used for next command. Since it is time consuming process, no need to run it
each time, and can be accessed from ```/home/users/nandan/backup/flatntuple_step5_RAW2DIGI_L1Reco_RECO.root```. For next steps, input for raw data is taken from the above location.
If you want to change this location, change it [here](https://github.com/saswatinandan/cmssw/blob/saswati/RecoLocalTracker/SiStripClusterizer/test/run_flatNtuplizer.py#L5)
accordingly.

## To run single bit
```sh
   cd RecoLocalTracker/SiStripClusterizer/test
   python3 run_cmsDriver.py -b 16 -n 100 -t 20
```
- `"b"`: which bit to be studied.
- `"n"`: # of events to be run.
- `"t"`: # how many threads to be used

Above command will create root files for ```step2, step3, and flatntuples ``` with cluster, track and jet information for ```16 bit``` and produce the plots for the same in ```test```
directory.

## To run all bit at a time
```sh
   cd RecoLocalTracker/SiStripClusterizer/test
   python3 run_allbit.py -b 16 14 12 10 8 6 4 -n 100 -t 20 -r 1
```
Above command will create root files in parallel for ```step2, step3, and flatntuples ``` with cluster, track and jet information for ```all bits``` and produce the plots
for each bit in ```output_${bit}bit``` folder inside the ```test``` directory and also the comparison plots for all the bits in ```test``` directory.

- `"b"`: which bit to be studied.
- `"n"`: # of events to be run.
- `"t"`: how many threads to be used.
- `"r"`: above command will create separate folders for separate bits. Once everything is made, if you want you can delete those folders by setting r to 1.
