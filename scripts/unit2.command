#!/bin/bash

# =========================================================================
# DESCRIPTION
# starts up MicroMacro app with certain params

# =========================================================================
# PARAMETERS
 
# initial wait time before starting commands of this script in seconds
# to make sure that OS X does whatever it needs to do after startup
initial_wait_time=1

# =========================================================================

echo "Starting MicroMacro"
#change to working directory
dir=${0%/*}
if [ -d "$dir" ]; then
  cd "$dir"
fi
echo "Waiting to start script..."
# wait before starting commands of this script in seconds
sleep $initial_wait_time
# start master application
echo "Launching application"
# logLevel: 0=verbose, 1=notice, 2=warning, 3=error, 4=fatal, 5=silent
../slitscan/bin/slitscan.app/Contents/MacOS/slitscan --args -id=2 -logLevel=0 -logToFile=false

exit 0