#!/bin/sh

#---------------------------------------------------------------------------
#
#                PLATFORM-SPECIFIC DEFINITIONS. 
#
#---------------------------------------------------------------------------

#---------------------------------------------------------------------------
# Items in this file will need to be changed for each platform.
# NF= Platform Numa factor
# NLINKS= Number of links of each node
# RB= Remote bandwidth
# LL= local latency
# BD= local bandwidth
#---------------------------------------------------------------------------
read n < /tmp/numacost.minas
read n < /tmp/numacost.minas
BD=$n;
export $BD;
read n < /tmp/numacost.minas
read n < /tmp/numacost.minas
LL=$n
export $LL;
read n < /tmp/numacost.minas
read n < /tmp/numacost.minas
RB=$n;
export $RB;
read n < /tmp/numacost.minas
read n < /tmp/numacost.minas
NF=$n;
export $NF;
NLINKS=3;
export NLINKS;
