#!/bin/bash
####################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     full_pass.sh
#  Authors:  Mason Remy
#
####################################################################################################

chmod +x import_all.sh
./import_all.sh

chmod +x build_all.sh
./build_all.sh

chmod +x run_all.sh
./run_all.sh
