import sys

import h5py
import vtk

import numpy as np

from vtk.util import numpy_support

def to_vtk(n_array, dim, spacing=(1,1,1)):
    dz, dy, dx = dim
    data_type = n_array.dtype

    v_image = numpy_support.numpy_to_vtk(n_array)

    # Generating the vtkImageData
    image = vtk.vtkImageData()
    image.SetDimensions(dx, dy, dz)
    image.SetOrigin(0, 0, 0)
    image.SetSpacing(spacing)
    image.SetNumberOfScalarComponents(1)
    image.SetExtent(0, dx -1, 0, dy -1, 0, dz - 1)
    # getattr(image, NUMPY_TO_VTK_TYPE[n_array.dtype.name])()
    image.SetScalarType(numpy_support.get_vtk_array_type(n_array.dtype))
    image.AllocateScalars()
    image.GetPointData().SetScalars(v_image)

    return image

def save_to_vti(imagedata, file_output):
    print "Saving"
    w = vtk.vtkXMLImageDataWriter()
    w.SetInput(imagedata)
    w.SetFileName(file_output)
    w.Write()
    print "Saved"

def main():
    m_input = sys.argv[1]
    v_output = sys.argv[2]

    try:
        spacing = [float(i) for i in sys.argv[3:]]
    except:
        spacing = 1.0, 1.0, 1.0

    m_image = h5py.File(m_input, 'r')
    data = np.array(m_image['dset'])
    dz, dy, dx = data.shape
    v_image = to_vtk(data.reshape(dz*dy*dx), data.shape, spacing)
    save_to_vti(v_image, v_output)

if __name__ == '__main__':
    main()  
