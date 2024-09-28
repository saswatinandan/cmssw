
export bit=$1
export env=$2
export thread=$3
export remove=$4

export current_dir=$(pwd)
echo 'current dir: ', $current_dir

if  ! [ -d "output_${bit}" ]; then
  eval "mkdir output_${bit}"
fi

if [ -d "/home/users/$(whoami)/${bit}" ]; then
  eval "rm -rf /home/users/$(whoami)/${bit}"
fi

eval "mkdir /home/users/$(whoami)/${bit}"
eval "cd /home/users/$(whoami)/${bit}"

eval ". /cvmfs/cms.cern.ch/cmsset_default.sh"
eval "cmsrel CMSSW_14_0_11"
eval "cd CMSSW_14_0_11/src/"
eval "cmsenv"
eval "git cms-init"
eval "git pull git@github.com:saswatinandan/cmssw.git saswati"
eval "git cms-addpkg Configuration/Eras DataFormats/SiStripCluster"
eval "git cms-addpkg RecoLocalTracker/SiStripClusterizer" 
eval "scram b -j 8"
eval "cd RecoLocalTracker/SiStripClusterizer/test"

export tmp_dir=$(pwd)
echo 'tmp dir: ', $tmp_dir

python3 run_cmsDriver.py -b $bit -n $env -t $thread

eval "cd $current_dir/output_${bit}"
eval "cp $tmp_dir/*png ."
eval "cp $tmp_dir/step*root ."
eval "cp $tmp_dir/*study*root ."
eval "cp $tmp_dir/outputPhy*root ."
eval "cp $tmp_dir/*log ."

echo "remove", $remove
if [ $remove -eq 1 ]; then
  eval "rm -rf ~/$bit"
fi
