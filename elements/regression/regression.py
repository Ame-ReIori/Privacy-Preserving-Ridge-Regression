#!/usr/bin/env python3

'''Ridge Regression test using Python (pandas & numpy)'''

__author__ = "William CLOT, www.github.com/williamclot; Camille PLAYS, www.github.com/camilleplays"
__license__ = "MIT"
__date__ = "22/10/2018"

import pandas as pd
import numpy as np
import math

##---------* Functions *----------##

class Regression:
    def __init__(self, X, Y, lamb=0.1):
        self.Y = Y
        self.X = X

        # A and b from X and Y
        self.A, self.Xt = self.getA(X, lamb)
        self.b = self.getb(self.Xt, self.Y)

        # Cholesky Decomposition
        self.L, self.Lt = self.cholesky0(self.A)

        # Result beta of Regression
        self.beta = self.back_substitution_upper(self.Lt, self.back_substitution_lower(self.L, self.b))

    def getA(self, X, lamb):
        '''
        Get A from X and lambda
        '''
        d = len(X[0])
        I = np.eye(d)
        Xt = np.transpose(X)

        A = np.dot(Xt, X)+lamb*I

        return A, Xt

    def getb(self, Xt, Y):
        '''
        Get b from Xt and Y
        '''
        b = np.dot(Xt, Y)

        return b


    def cholesky0(self, A):
        """
        Performs a Cholesky decomposition of on symmetric, pos-def A.
        Returns lower-triangular L (full sized, zeroed above diag)
        """
        n = A.shape[0]
        L = np.zeros_like(A)

        # Perform the Cholesky decomposition
        for row in range(n):
            for col in range(row+1):
                tmp_sum = np.dot(L[row,:col], L[col,:col])
                if (row == col): # Diagonal elements
                    L[row, col] = math.sqrt(max(A[row,row] - tmp_sum, 0))
                else:
                    L[row,col] = (1.0 / L[col,col]) * (A[row,col] - tmp_sum)
        return L, np.transpose(L)

    def back_substitution_upper(self, LT, b):
        new = np.copy(b)
        d = len(new)
        Y = np.zeros((d,1))
        Y[d-1][0]=new[d-1][0]/LT[d-1][d-1]
        for i in range(d-2,-1,-1):
            for j in range(d-1,i,-1):
                new[i][0]=new[i][0]-(LT[i][j]*Y[j][0])
            Y[i][0]=new[i][0]/LT[i][i]

        return Y

    def back_substitution_lower(self, L, Y):
        new = np.copy(Y)
        d = len(new)
        beta = np.zeros((d,1))
        beta[0][0]=new[0][0]/L[0][0]
        for i in range(1,d):
            for j in range(0,i):
                new[i][0]=new[i][0]-(L[i][j]*beta[j][0])
            beta[i][0]=new[i][0]/L[i][i]

        return beta



