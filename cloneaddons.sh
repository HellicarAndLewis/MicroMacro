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
cd ofxBlackmagic
git checkout bef9cb5547198fe842fce9d631b05d2be28a2322
cd ..

${CLONE}obviousjim/ofxSlitScan
cd ofxSlitScan
git checkout 78cdd0dc9fff4eac93406c13057064e5a3abb8b3
cd ..

${CLONE}Flightphase/ofxCvOpticalFlowLK
cd ofxCvOpticalFlowLK
git checkout f918ed5379b9b060a80f4c4c93d40aa96d8bdb1f
cd ..

${CLONE}armadillu/ofxRemoteUI
cd ofxRemoteUI
git checkout 3c73fbed1e004c8bd0c542985088b14a6969a648
cd ..

${CLONE}memo/ofxMSAInteractiveObject
cd ofxMSAInteractiveObject
git checkout 13eb17852016e9aa2c2eaac0d20613709b609a10
cd ..

${CLONE}outsidecontext/ofxArgs


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