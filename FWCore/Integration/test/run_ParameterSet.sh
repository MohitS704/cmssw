#!/bin/bash

test=testParameterSet

function die { echo Failure $1: status $2 ; exit $2 ; }

LOCAL_TEST_DIR=${SCRAM_TEST_PATH}

# Intentionally fails because of an illegal parameter in a module.
  echo ${test} ------------------------------------------------------------
  cmsRun ${LOCAL_TEST_DIR}/${test}_cfg.py 2> ${test}.txt
  grep "Illegal parameter" ${test}.txt || die "cmsRun ${test}_cfg.py" $?

# Intentionally fails because of an illegal parameter in a source.
  echo testIllegalSourceParameter_cfg.py ------------------------------------------------------------
  cmsRun ${LOCAL_TEST_DIR}/testIllegalSourceParameter_cfg.py 2> testIllegalSourceParameter_cfg.txt
  grep "Illegal parameter" testIllegalSourceParameter_cfg.txt || die "cmsRun testIllegalSourceParameter_cfg.py" $?

# Intentionally fails because of an illegal parameter in a service.
  echo testIllegalServiceParameter_cfg.py ------------------------------------------------------------
  cmsRun ${LOCAL_TEST_DIR}/testIllegalServiceParameter_cfg.py 2> testIllegalServiceParameter_cfg.txt
  grep "Illegal parameter" testIllegalServiceParameter_cfg.txt || die "cmsRun testIllegalServiceParameter_cfg.py" $?

# Note that DoodadESSource and WhatsItESProducer classes contain little tests for
# validation of ESSource's and ESProducer's. They are not run here, but thought
# this comment would be helpful for anyone trying to find those tests.

# Auto generate a cfi file
  echo edmWriteConfigs ------------------------------------------------------------
  edmWriteConfigs pluginFWCoreIntegrationTestPSet.so || die "edmWriteConfigs pluginTestProducerWithPSetDesc.so" $?

# Make sure we can run using the cfi file generated in the previous process
  echo cmsRun runAutoGeneratedCfi_cfg.py ------------------------------------------------------------
  cmsRun ${LOCAL_TEST_DIR}/runAutoGeneratedCfi_cfg.py || die "cmsRun runAutoGeneratedCfi_cfg.py" $?

# Compare the cfi file to a reference file to ensure it is correct
  mkdir testcfi
  cp ${LOCAL_TEST_DIR}/cfi_dumpPython.py testcfi
  cp testProducerWithPsetDesc_cfi.py testcfi
  cp ProducerWithPSetDesc.py testcfi
  PYTHON3PATH=${PWD}:${PYTHON3PATH} python3 -m testcfi.cfi_dumpPython > dumpPython.log
  python3 ${LOCAL_TEST_DIR}/unit_test_outputs/testProducerWithPsetDesc_cfi.py > dumpPython_ref.log
  diff ./dumpPython.log dumpPython_ref.log || die "comparing testProducerWithPsetDesc_cfi.py" $?

# This time try a cfi without any of the required parameters to test that we properly insert
# required missing parameters into the ParameterSet
  echo cmsRun runNotAutoGeneratedCfi_cfg.py ------------------------------------------------------------
  cmsRun ${LOCAL_TEST_DIR}/runNotAutoGeneratedCfi_cfg.py || die "cmsRun runNotAutoGeneratedCfi_cfg.py" $?

# Compare the output of the PSetAnalyzer with a reference file
  echo cmsRun testPSetAnalyzer_cfg.py ------------------------------------------------------------
  cmsRun ${LOCAL_TEST_DIR}/testPSetAnalyzer_cfg.py 2>&1 | grep " PSet test " > testPSetAnalyzer.txt
  diff ./testPSetAnalyzer.txt ${LOCAL_TEST_DIR}/unit_test_outputs/testPSetAnalyzer.txt || die "comparing testPSetAnalyzer.txt" $?

# Print human readable from the ParameterSetDescription
  echo edmPluginHelp -p ProducerWithPSetDesc ------------------------------
  edmPluginHelp -p ProducerWithPSetDesc &> testProducerWithPsetDesc_doc.txt || die "edmPluginHelp -p ProducerWithPSetDesc" $?
  diff ./testProducerWithPsetDesc_doc.txt ${LOCAL_TEST_DIR}/unit_test_outputs/testProducerWithPsetDesc_doc.txt || die "comparing testProducerWithPsetDesc_doc.txt" $?

# Print human readable from the ParameterSetDescription, test comments in ParameterDescription constructor arguments
  echo edmPluginHelp -p TestDescriptionComments ------------------------------
  edmPluginHelp -p TestDescriptionComments &> testDescriptionComments_doc.txt || die "edmPluginHelp -p TestDescriptionComments" $?
  diff ./testDescriptionComments_doc.txt ${LOCAL_TEST_DIR}/unit_test_outputs/testDescriptionComments_doc.txt || die "comparing testDescriptionComments_doc.txt" $?

# Print human readable from the ParameterSetDescription, brief format
  echo edmPluginHelp -p ProducerWithPSetDesc -b ---------------------------
  edmPluginHelp -p ProducerWithPSetDesc -b &> testProducerWithPsetDesc_briefdoc.txt || die "edmPluginHelp -p ProducerWithPSetDesc -b" $?
  diff ./testProducerWithPsetDesc_briefdoc.txt ${LOCAL_TEST_DIR}/unit_test_outputs/testProducerWithPsetDesc_briefdoc.txt || die "comparing testProducerWithPsetDesc_briefdoc.txt" $?

  echo edmPluginHelp -t options -b ---------------------------
  edmPluginHelp -t options -b &> testEdmPluginHelpOptions.txt || die "edmPluginHelp -t options -b" $?
  grep "numberOfThreads[[:blank:]]\+untracked uint32[[:blank:]]\+1" testEdmPluginHelpOptions.txt || die "testing edmPluginHelp -t options -b" $?

# Test for errors and success when importing a restricted file in various ways
  echo cmsRun importRestrictions1.py ------------------------------------------------------------
  cmsRun ${LOCAL_TEST_DIR}/importRestrictions1.py 2> importRestrictions1.txt
  grep "Event 2" importRestrictions1.txt || die " cmsRun importRestrictions1.py" $?
  echo cmsRun importRestrictions2.py ------------------------------------------------------------
  cmsRun ${LOCAL_TEST_DIR}/importRestrictions2.py 2> importRestrictions2.txt
  grep "ImportError" importRestrictions2.txt || die " cmsRun importRestrictions2.py" $?
  echo cmsRun importRestrictions3.py ------------------------------------------------------------
  cmsRun ${LOCAL_TEST_DIR}/importRestrictions3.py 2> importRestrictions3.txt
  grep "Event 2" importRestrictions3.txt || die " cmsRun importRestrictions3.py" $?

exit 0
