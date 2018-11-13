#!/usr/bin/env python3

'''Ridge Regression test using Python (pandas & numpy)'''

__author__ = "William CLOT, www.github.com/williamclot; Camille PLAYS, www.github.com/camilleplays"
__license__ = "MIT"
__date__ = "13/11/2018"

import pandas as pd
from phe import paillier
import numpy as np
import math
from termcol import termcol as tc

##---------* Functions *----------##

class CSP:
    def __init__(self, verbose=False):
        # Programm parameters
        self.verbose = verbose

        if (self.verbose):
            print(tc.WARNING+"Initiating the CSP..."+tc.ENDC)

        # Generate the public and private key used for Paillier encryption and decryption
        self.public_key, private_key = paillier.generate_paillier_keypair() 






