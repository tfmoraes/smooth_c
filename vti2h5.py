import sys
import vtk
import h5py

from vtk.util import numpy_support

def to_h5(fname, outname):
    r = vtk.vtkXMLImageDataReader()
    r.SetFileName(fname)
    r.Update()

    o = r.GetOutput()
    x, y, z = o.GetDimensions()
    
    m = numpy_support.vtk_to_numpy(o.GetPointData().GetScalars())
    m.shape = (z, y, x)
    #  m[m>0] = 1
    print m.dtype, m.shape

    mh5 = h5py.File(outname, 'w')
    mh5.create_dataset('spacing', data=o.GetSpacing());
    mh5.create_dataset('data', shape=m.shape, dtype=m.dtype)
    d = mh5['data']
    d[:] = m
    mh5.flush()
    mh5.close()


def main():
    to_h5(sys.argv[1], sys.argv[2])


if __name__ == '__main__':
    main()
