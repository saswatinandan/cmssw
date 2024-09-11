## Command to add track information for raw
```sh
cmsDriver.py step5 --conditions auto:run3_data_prompt -s RAW2DIGI,L1Reco,RECO --datatier RECO --eventcontent RECO --data --process reRECO --scenario pp -n 100 --repacked --era Run3 --nThreads 254 --filein /store/data/Run2024F/Muon0/RAW-RECO/ZMu-PromptReco-v1/000/382/216/00000/aadd1ab9-4eb8-4fb2-ac62-bdd1bebe882e.root
```
One should run the above command first as the output of the previous command will be used for next command

## Command to create ntuples using cmsDriver command for rawprime, to create flat ntuples and do object comparison between raw and rawprime 
 Command
 ```sh
   python3 run_cmsDriver.py
```
Above code takes the argument `b` which is to set the `bit to be studied`. `Default value` is set to `8bit`
To change the default value run following:
```sh
python3 run_cmsDriver.py -b 16bit
```
Above command will produce flat ntuples and produce the comparison plots for track and jets between raw and rawprime for the input bit provided in the above command.



