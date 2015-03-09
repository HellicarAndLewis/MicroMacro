#!/bin/bash

# Clones dependencies
# !Run this script from the project root!
# In terminal cd to project root then "sh cloneaddons.sh"

# switch to addons directory
cd ../../addons

# passing no arguments means read only
if [ -z $1 ]; then
CLONE="git clone https://github.com/"
echo "Cloning read-only"
else
CLONE="git clone git@github.com:"
echo "Cloning writable"
fi

# git clone specific revision of the addons
${CLONE}kylemcdonald/ofxBlackmagic
# cd ofxBlackmagic
# git checkout ADD_REVISION_HERE
# cd ..

${CLONE}obviousjim/ofxSlitScan
# cd ofxSlitScan
# git checkout ADD_REVISION_HERE
# cd ..

${CLONE}Flightphase/ofxCvOpticalFlowLK

# potentially useful:
# ${CLONE}rezaali/ofxUI
# ${CLONE}neilmendoza/ofxPostProcessing
# ${CLONE}obviousjim/ofxMSATimer
# ${CLONE}arturoc/ofxTween
# ${CLONE}satoruhiga/ofxFastFboReader
# ${CLONE}kylemcdonald/ofxCameraFilter
# ${CLONE}kylemcdonald/ofxCv

# final step is to cd back to the start dir
cd ../apps/MicroMacro