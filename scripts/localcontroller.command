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
../controller/bin/controller.app/Contents/MacOS/controller --args -address="10.41.14.255"

exit 0