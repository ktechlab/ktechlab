/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "matrix.h"

#include <cmath>
#include <iostream>

Matrix::Matrix(CUI n, CUI m)
    : m_n(n)
{
    unsigned int size = m_n + m;

    m_mat = new QuickMatrix(size);
    m_lu = new QuickMatrix(size);

    m_y = new double[size];
    m_inMap = new int[size];

    for (unsigned int i = 0; i < size; i++)
        m_inMap[i] = i;
}

Matrix::~Matrix()
{
    delete m_mat;
    delete m_lu;
    delete[] m_y;
    delete[] m_inMap;
}

void Matrix::swapRows(CUI a, CUI b)
{
    if (a == b)
        return;
    m_mat->swapRows(a, b);

    const int old = m_inMap[a];
    m_inMap[a] = m_inMap[b];
    m_inMap[b] = old;

    max_k = 0;
}

void Matrix::performLU()
{
    unsigned int n = m_mat->size_m();
    if (n == 0)
        return;

    // Copy the affected segment to LU
    for (uint i = max_k; i < n; i++) {
        for (uint j = max_k; j < n; j++) {
            (*m_lu)[i][j] = (*m_mat)[i][j];
        }
    }

    // LU decompose the matrix, and store result back in matrix
    for (uint k = 0; k < n - 1; k++) {
        double *const lu_K_K = &(*m_lu)[k][k];
        unsigned foo = std::max(k, max_k) + 1;

        // detect singular matrixes...
        if (std::abs(*lu_K_K) < 1e-10) {
            if (*lu_K_K < 0.)
                *lu_K_K = -1e-10;
            else
                *lu_K_K = 1e-10;
        }
        // #############

        for (uint i = foo; i < n; i++) {
            (*m_lu)[i][k] /= *lu_K_K;
        }

        for (uint i = std::max(k, max_k) + 1; i < n; i++) {
            const double lu_I_K = (*m_lu)[i][k];
            if (std::abs(lu_I_K) > 1e-12) {
                m_lu->partialSAF(k, i, foo, -lu_I_K);
            }
        }
    }

    max_k = n;
}

void Matrix::fbSub(QuickVector *b)
{
    unsigned int size = m_mat->size_m();

    for (uint i = 0; i < size; i++) {
        m_y[m_inMap[i]] = (*b)[i];
    }

    // Forward substitution
    for (uint i = 1; i < size; i++) {
        double sum = 0;
        for (unsigned int j = 0; j < i; j++) {
            sum += (*m_lu)[i][j] * m_y[j];
        }
        m_y[i] -= sum;
    }

    // Back substitution
    m_y[size - 1] /= (*m_lu)[size - 1][size - 1];
    for (int i = size - 2; i >= 0; i--) {
        double sum = 0;
        for (uint j = i + 1; j < size; j++) {
            sum += (*m_lu)[i][j] * m_y[j];
        }
        m_y[i] -= sum;
        m_y[i] /= (*m_lu)[i][i];
    }

    // I think we don't need to reverse the mapping because we only permute rows, not columns.
    for (uint i = 0; i < size; i++)
        (*b)[i] = m_y[i];
}

void Matrix::displayLU()
{
    uint n = m_mat->size_m();
    for (uint _i = 0; _i < n; _i++) {
        uint i = m_inMap[_i];
        // 		uint i = _i;
        for (uint j = 0; j < n; j++) {
            if (j > 0 && (*m_lu)[i][j] >= 0)
                std::cout << "+";
            std::cout << (*m_lu)[i][j] << "(" << j << ")";
        }
        std::cout << std::endl;
    }
    std::cout << "m_inMap:    ";
    for (uint i = 0; i < n; i++) {
        std::cout << i << "->" << m_inMap[i] << "  ";
    }
    std::cout << std::endl;
    /*cout << "m_outMap:   ";
    for ( uint i=0; i<n; i++ )
    {
        cout << i<<"->"<<m_outMap[i]<<"  ";
    }
    cout << endl;*/
}
