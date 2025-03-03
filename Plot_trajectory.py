# -*- coding: utf-8 -*-
"""
Created on Mon May 27 11:41:07 2024

@author: michael.murzin
"""

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import datetime

initial_filename = 'traject.txt'
downloaded_traject_filename = 'downloaded_nodes.txt'

nodes = np.loadtxt('1.txt')
points = np.loadtxt('downloaded_nodes')

def analyze(interp_nodes, points):
    x = interp_nodes[:,0]
    y = interp_nodes[:,1]
    
    xx = np.array( range(x[-1]) )
    yy = np.interp(xx, x, y)
    
    diff = yy - points
    max_diff = np.max(diff)
    min_diff = np.min(diff)
    
    print( f'Min diff = {min_diff:.3f}, max diff = {max_diff:.3f}' )
    
    return yy
    

def visualize(yy, points):
    ax1 = plt.subplot(211)
    plt.plot(points, label='uC')
    plt.grid()
    
    plt.plot( yy, 'r', label='PC' )

    plt.legend(loc='upper left')
    
    plt.subplot(212, sharex=ax1)
    plt.plot(yy - points, label='PC - uC')
    plt.grid()
    plt.legend(loc='upper left')
    
    plt.show()
    current_datetime = datetime.datetime.now()
    plt.savefig(f'Traject_comarison_{current_datetime}.png')

yy = analyze(nodes, points)
visualize(yy,points)