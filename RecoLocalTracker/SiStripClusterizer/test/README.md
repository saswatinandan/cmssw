### Compile the utilities codes for cluster, dead-strip info dumping ([sep19_2_1_dump_rawprime](sep19_2_1_dump_rawprime.cc), [sep19_2_2_dump_raw](sep19_2_2_dump_raw.cc), [sep19_3_dump_deadStrips](sep19_3_dump_deadStrips.cc))
```bash
scram b -j8
```

### Run the configuration files to get the raw-rawprime matched-event cluster collection, dead strip info
```bash
# chmod 755 script/execute
script/execute
```
> `script/execute` documents the common lumi section files with raw & rawprime datasets. The common lumis and common events are provided in this [google spreadsheet](https://docs.google.com/spreadsheets/d/1z3M-orW7XHjjgSLhbj9HaosmUdPaSw6ozqj9I9xBVDk/edit?usp=sharing)

### Do cluster matching, plot the cluster distributions
```bash
cd rootMacro/
# chmod 755 execute.sh
./execute.sh
```
