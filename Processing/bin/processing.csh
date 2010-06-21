if ($?CMSSW_BASE) then 
  setenv MIT_PROD_DIR "$CMSSW_BASE/src/MitProd/Processing"
  setenv PATH         "$CMSSW_BASE/src/MitProd/Processing/bin:${PATH}"
  setenv PYTHONPATH   "$CMSSW_BASE/src/MitProd/Processing/python:${PYTHONPATH}"
else
  echo ""
  echo " Setting up MitProd Processing failed! (\$CMSSW_BASE = is empty)."
  echo ""
endif
