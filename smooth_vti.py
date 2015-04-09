import numpy as np
import smooth

import sys
import vtk
import h5py

from vtk.util import numpy_support

def vti_to_nparray(fname):
    r = vtk.vtkXMLImageDataReader()
    r.SetFileName(fname)
    r.Update()

    o = r.GetOutput()
    x, y, z = o.GetDimensions()
    
    m = numpy_support.vtk_to_numpy(o.GetPointData().GetScalars())
    m.shape = (z, y, x)
    m[:] = (m > 127) * 1
    
    return m, o.GetSpacing()


def to_vtk(n_array, dim, spacing=(1,1,1)):
    dz, dy, dx = dim
    data_type = n_array.dtype
    n_array.shape = dz*dy*dx

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

    n_array.shape = dz,dy,dx

    return image


def save_to_vti(imagedata, file_output):
    print "Saving"
    w = vtk.vtkXMLImageDataWriter()
    w.SetInput(imagedata)
    w.SetFileName(file_output)
    w.Write()
    print "Saved"


def main():
    img, spacing = vti_to_nparray(sys.argv[1])
    out_img = smooth.smoothpy(np.array(img[1:, 1:, 1:]), int(sys.argv[3]))
    vtk_img = to_vtk(out_img, out_img.shape, spacing)
    save_to_vti(vtk_img, sys.argv[2])

if __name__ == '__main__':
    main()
