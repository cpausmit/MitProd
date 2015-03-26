if [ -z $CMSSW_BASE ]
then
  echo ""
  echo " Setting up MitProd Processing failed! (\$CMSSW_BASE = is empty)."
  echo ""
else
  export MIT_PROD_DIR="$CMSSW_BASE/src/MitProd/Processing"
  export PATH="$HOME/bin:$CMSSW_BASE/src/MitProd/Processing/bin:${PATH}"
  export PYTHONPATH="$CMSSW_BASE/src/MitProd/Processing/python:${PYTHONPATH}"
  pVersion=`python --version 2>&1`
  pLocalVersion=`/usr/bin/python --version 2>&1`
  if [ "$pVersion" == "$pLocalVersion" ]
  then
    addedPath=`echo /usr/lib64/python*/site-packages`
    export PYTHONPATH="${PYTHONPATH}:$addedPath"
  else
    echo " Local python version not compatible with CMSSW release."
    echo " This might cause issues."
  fi

fi
