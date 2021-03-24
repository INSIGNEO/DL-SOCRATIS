# Copyright 2019, Wenjia Bai. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the 'License');
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an 'AS IS' BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ==============================================================================
import os
import math
import numpy as np
import nibabel as nib
import cv2
import vtk
import pandas as pd
import matplotlib.pyplot as plt
from vtk.util import numpy_support
from scipy import interpolate
#import skimage

if __name__ == '__main__':
    if len(sys.argv) < 5:
        print('Usage: {0} contour(.vtk) T1(int) dt1(float) output_dir'.format(sys.argv[0]))
        exit(1)

    contour_name = sys.argv[1]
    T1 = sys.argv[2]
    dt1 = sys.argv[3]
    output_name =sys.argv[4]
    evaluate_strain_by_length(contour_name_stem, T1, dt1, output_name)



def evaluate_strain_by_length(contour_name_stem, T, dt, output_name_stem):
    """ Calculate the strain based on the line length """
    # Read the polydata at the first time frame (ED frame)
    fr = 0
    reader = vtk.vtkPolyDataReader()
    reader.SetFileName('{0}{1:02d}.vtk'.format(contour_name_stem, fr))
    reader.Update()
    poly = reader.GetOutput()
    points = poly.GetPoints()

    # Calculate the length of each line
    lines = poly.GetLines()
    lines_aha = poly.GetCellData().GetArray('Segment ID')
    lines_dir = poly.GetCellData().GetArray('Direction ID')
    n_lines = lines.GetNumberOfCells()
    length_ED = np.zeros(n_lines)
    seg_id = np.zeros(n_lines)
    dir_id = np.zeros(n_lines)

    lines.InitTraversal()
    for i in range(n_lines):
        ids = vtk.vtkIdList()
        lines.GetNextCell(ids)
        p1 = np.array(points.GetPoint(ids.GetId(0)))
        p2 = np.array(points.GetPoint(ids.GetId(1)))
        d = np.linalg.norm(p1 - p2)
        seg_id[i] = lines_aha.GetValue(i)
        dir_id[i] = lines_dir.GetValue(i)
        length_ED[i] = d

    # For each time frame, calculate the strain, i.e. change of length
    table_strain = {}
    table_strain['radial'] = np.zeros((17, T))
    table_strain['circum'] = np.zeros((17, T))

    for fr in range(0, T):
        # Read the polydata
        reader = vtk.vtkPolyDataReader()
        filename = '{0}_{1:02d}.vtk'.format(contour_name_stem, fr)
	filename2='{0}_{1:02d}.vtk'.format(output_name_stem, fr)
        reader.SetFileName(filename)
        reader.Update()
        poly = reader.GetOutput()
        points = poly.GetPoints()

        # Calculate the strain for each line
        lines = poly.GetLines()
        n_lines = lines.GetNumberOfCells()
        strain = np.zeros(n_lines)
        vtk_strain = vtk.vtkFloatArray()
        vtk_strain.SetName('Strain')
        lines.InitTraversal()
        for i in range(n_lines):
            ids = vtk.vtkIdList()
            lines.GetNextCell(ids)
            p1 = np.array(points.GetPoint(ids.GetId(0)))
            p2 = np.array(points.GetPoint(ids.GetId(1)))
            d = np.linalg.norm(p1 - p2)

            # Strain of this line (unit: %)
            strain[i] = (d - length_ED[i]) / length_ED[i] * 100
            vtk_strain.InsertNextTuple1(strain[i])

        # Save the strain array to the vtk file
        poly.GetCellData().AddArray(vtk_strain)
        writer = vtk.vtkPolyDataWriter()
        writer.SetInputData(poly)
        writer.SetFileName(filename2)
        writer.Write()
        os.system('sed -i "1s/4.1/4.0/" {0}'.format(filename))

        # Calculate the segmental and global strains
        for i in range(0, 16):
            table_strain['radial'][i, fr] = np.mean(strain[(seg_id == (i + 1)) & (dir_id == 1)])
            table_strain['circum'][i, fr] = np.mean(strain[(seg_id == (i + 1)) & (dir_id == 2)])
        table_strain['radial'][-1, fr] = np.mean(strain[dir_id == 1])
        table_strain['circum'][-1, fr] = np.mean(strain[dir_id == 2])

    for c in ['radial', 'circum']:
        # Save into csv files
        index = [str(x) for x in np.arange(1, 17)] + ['Global']
        column = np.arange(0, T) * dt * 1e3
        df = pd.DataFrame(table_strain[c], index=index, columns=column)
        df.to_csv('{0}_{1}.csv'.format(output_name_stem, c))

