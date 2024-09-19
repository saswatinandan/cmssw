
export bit=$1
export env=$2
export thread=$3
export remove=$4

export current_dir=$(pwd)
echo $current_dir

if  ! [ -d "for_$bit" ]; then
  eval "mkdir for_$bit"
fi

if [ -d "/home/users/$(whoami)/$bit" ]; then
  eval "rm -rf /home/users/$(whoami)/$bit"
fi

eval "mkdir /home/users/$(whoami)/$bit"
eval "cd /home/users/$(whoami)/$bit"

eval ". /cvmfs/cms.cern.ch/cmsset_default.sh"
eval "cmsrel CMSSW_14_1_0_pre6"
eval "cd CMSSW_14_1_0_pre6/src/"
eval "cmsenv"
eval "git cms-init"
eval "git pull git@github.com:saswatinandan/cmssw.git saswati"
eval "git cms-addpkg Configuration/Eras DataFormats/SiStripCluster"
eval "git cms-addpkg RecoLocalTracker/SiStripClusterizer" 
eval "scram b -j 8"
eval "cd RecoLocalTracker/SiStripClusterizer/test"
eval "cp $current_dir/step5_RAW2DIGI_L1Reco_RECO.root ."

export tmp_dir=$(pwd)
echo $tmp_dir

python3 run_cmsDriver.py -b $bit -n $env -t $thread

eval "cd $current_dir/for_$bit"
eval "cp $tmp_dir/*png ."
echo $current_dir, $tmp_dir
eval "cp $tmp_dir/*pdf ."

echo "remove", $remove
if [ $remove ]; then
  eval "rm -rf ~/$bit"
fi
