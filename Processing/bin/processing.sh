if [ -z $CMSSW_BASE ]
then
  echo ""
  echo " Setting up MitProd Processing failed! (\$CMSSW_BASE = is empty)."
  echo ""
else
  export PATH="$CMSSW_BASE/src/MitProd/Processing/bin:${PATH}"
  export PYTHONPATH="$CMSSW_BASE/src/MitProd/Processing/python:${PYTHONPATH}"
fi
