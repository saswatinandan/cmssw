#!/bin/bash
outdir_step2="/scratch/nandan/inputfile_for_prehlt/$1/$2/step2"
mkdir -p $outdir_step2   # create output directory if it doesn't exist
outdir_reco="/scratch/nandan/inputfile_for_prehlt/$1/$2/step_reco"
mkdir -p $outdir_reco   # create output directory if it doesn't exist
outdir_muon="/scratch/nandan/inputfile_for_prehlt/$1/$2/muon_analyzer/zmass"
mkdir -p $outdir_muon   # create output directory if it doesn't exist
index=1 
files=${1}.root
ls /gpfs/ddn/srm/cms/store/user/snandan/EphemeralHLTPhysics4/step_reco_rawp_wo_chargecut/260310_145359/0000/*root > $files
while read f; do
	#if (( index < 18 )); then
	#   index=$((index + 1))
        #   continue
	#fi
	outfile_step2="${outdir_step2}/step2_L1REPACK_HLT_${index}.root"
	#cmsRun step2_L1REPACK_HLT.py outputFile=$outfile_step2 inputFiles="file:$f"
        #cmsRun step2_L1REPACK_HLT.py outputFile=$outfile_step2 inputFiles="$f"
	outfile_reco="${outdir_reco}/step_reco_RAW2DIGI_L1Reco_RECO_${index}.root"
	#cmsRun step_reco_RAW2DIGI_L1Reco_RECO.py outputFile=$outfile_reco inputFiles="file:$outfile_step2"
	#cmsRun step_RAW2DIGI_L1Reco_RECO_PAT.py outputFile=$outfile_reco inputFiles="$f"
	#cmsRun step_RAW2DIGI_L1Reco_RECO_PAT_rawp.py outputFile=$outfile_reco inputFiles="file:$outfile_step2"
	outfile_muon="${outdir_muon}/muon_analyzer_${index}.root"
	#cmsRun muonanalyzer.py outputFile=$outfile_muon inputFiles="file:$outfile_reco"
	cmsRun muonanalyzer.py outputFile=$outfile_muon inputFiles="file:$f"
	if [ $? -ne 0 ]; then
           echo "Error in file $f, stopping loop"
	   break
	fi
	index=$((index + 1))
	done < $files
        #done < miniaod_xrootd.txt #miniaod.txt #dasgoclient -query="file dataset=/EphemeralHLTPhysics4/Run2025G-PromptReco-v1/MINIAOD" > miniaod.txt
        #done < raw.txt #

