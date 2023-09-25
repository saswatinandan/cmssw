### Compile the utilities codes for cluster, dead-strip info dumping ([sep19_2_1_dump_rawprime](sep19_2_1_dump_rawprime.cc), [sep19_2_2_dump_raw](sep19_2_2_dump_raw.cc), [sep19_3_dump_deadStrips](sep19_3_dump_deadStrips.cc))
```bash
scram b -j8
```

### Run the configuration files to get the raw-rawprime matched-event cluster collection, dead strip info
```bash
# chmod 755 script/execute
script/execute
```

### Do cluster matching, plot the cluster distributions
```bash
cd rootMacro/
# chmod 755 execute.sh
./execute.sh
```