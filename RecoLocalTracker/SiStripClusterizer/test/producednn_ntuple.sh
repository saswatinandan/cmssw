#!/bin/bash

inputfile=$1 #"/scratch/nandan/inputfile_for_prehlt/ephemeral/step_reco/step_reco_RAW2DIGI_L1Reco_RECO_1.root"
outdir_prehlt="/scratch/nandan/inputfile_for_prehlt/ephemeral_zerobias/dnn_ntuple/prehlt"
mkdir -p $outdir_prehlt
outdir_reco="/scratch/nandan/inputfile_for_prehlt/ephemeral_zerobias/dnn_ntuple/reco"
mkdir -p $outdir_reco
outdir_ntuple="/scratch/nandan/inputfile_for_prehlt/ephemeral_zerobias/dnn_ntuple/ntuple"
mkdir -p $outdir_ntuple

total_events=4130
chunk_size=100
index=1

for (( skip=0; skip<total_events; skip+=chunk_size ))
do
    outfile_prehlt="${outdir_prehlt}/prehlt_${index}.root"
    cmsRun step.py \
        inputFiles=file:$inputfile \
        outputFile=$outfile_prehlt \
        skipEvents=$skip \
        maxEvents=$chunk_size

    outfile_reco="${outdir_reco}/reco_${index}.root"
    outfile_prehlt="${outdir_prehlt}/prehlt_${index}_numEvent${chunk_size}.root"
    cmsRun step_reco_RAW2DIGI_L1Reco_RECO.py \
        inputFiles=file:${outfile_prehlt} \
        outputFile=$outfile_reco

    outfile_ntuple="${outdir_ntuple}/ntuple_${index}.root"
    cmsRun nn_treeProducer_raw.py \
        inputFiles=file:${outfile_reco} \
        outputFile=$outfile_ntuple
    ((index++))
done

