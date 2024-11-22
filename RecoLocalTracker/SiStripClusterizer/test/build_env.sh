
export barycenter_bit=$1
export width_bit=$2
export evn=$3
export thread=$4
export remove=$5
export output=/scratch/$(whoami)/$6
export tmp=/home/users/$(whoami)/tmp/$6

echo $tmp
echo $output
if  ! [ -d $output ]; then
  eval "mkdir -p $output"
fi

if [ -d ${tmp} ]; then
  echo "removing"
  eval "rm -rf ${tmp}"
fi

eval "mkdir -p ${tmp}"
eval "cd ${tmp}"

eval ". /cvmfs/cms.cern.ch/cmsset_default.sh"
eval "cmsrel CMSSW_14_0_11"
eval "cd CMSSW_14_0_11/src/"
eval "cmsenv"
eval "git cms-init"
eval "git pull git@github.com:saswatinandan/cmssw.git avgCharge"
eval "git cms-addpkg Configuration/Eras DataFormats/SiStripCluster"
eval "git cms-addpkg RecoLocalTracker/SiStripClusterizer" 
eval "scram b -j 8"
eval "cd RecoLocalTracker/SiStripClusterizer/test"

export current_dir=$(pwd)
echo 'curent dir: ', $current_dir

python3 run_cmsDriver.py -b $barycenter_bit -w $width_bit -n $evn -t $thread

eval "cd ${output}"
eval "cp $current_dir/*png ."
eval "cp $current_dir/*step*root ."
eval "cp $current_dir/*study*root ."
eval "cp $current_dir/outputPhy*root ."
eval "cp $current_dir/*log ."

echo "remove", $remove
if [ $remove -eq 1 ]; then
  echo $tmp
  eval "rm -rf ${tmp}"
fi
