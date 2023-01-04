#!BPY

"""
.bm1 (Binary Mesh version 1)
-------------
# No newlines, oterwise like .tm1 in binary (uint and float in 32 bit little-endian)
1 # uint number 1, to test endianness
M # uint
V # uint
T # uint
L # uint
M0R M0G M0B | M1R M1G M1B | ... # floats (count: M*3)
V0x V0y V0z | V1x V1y V1z | ... # floats (count: V*3)
T0a T0b T0c | T1a T1b T1c | ... # uints (count: T*3) (indices start at 0)
MT0 MT1 ... # uints (count: T)
L0a L0b | L1a L1b | ... # uints (count: L*2)
ML0 ML1 ... # uints (count: L)

"""

bl_info = {
    "name": "Export-bm1",
    "author": "Curved Ruler",
    "version": (1, 1),
    "blender": (2, 80, 0),
    "location": "File > Export",
    "description": "Export to format: Binary Mesh v1",
    "warning": "",
    "wiki_url": "",
    "tracker_url": "",
    "category": "Curved Ruler"
}

import struct
import bpy
from bpy_extras.io_utils import ExportHelper


# act must be [smaller_id, bigger_id]
def add_if_not_present (act, edge_list, col, cols):
    present = False
    for e in edge_list:
        if (e[0] == act[0] and e[1] == act[1]):
            present = True
            break
        
    if (not present):
        edge_list.append(act)
        cols.append(col)
        


class ExportTest(bpy.types.Operator, ExportHelper):
    """Export bm1"""
    
    bl_idname       = "example.bm1";
    bl_label        = "Export BM1";
    bl_options      = {'PRESET'};
    
    filename_ext    = ".bm1";
    
    def execute(self, context):
        
        materials = bpy.data.materials
        if (len(materials)) == 0:
            return {'E'}
            
        mat = []
        ver = []
        lin = []
        lc  = [] # line colors
        tri = []
        pc  = [] # polygon colors
        verti = 0
        
        for m in materials:
            mat.append([m.diffuse_color[0], m.diffuse_color[1], m.diffuse_color[2]])
        
        for ob in bpy.data.objects:
            if ob.select_get() and ob.type == 'MESH':
                mat_w = ob.matrix_world
                verti = len(ver)
                mesh = ob.to_mesh()
                
                for vertex in mesh.vertices:
                    pos = mat_w @ vertex.co
                    ver.append([pos.x, pos.y, pos.z])
                
                for polygon in mesh.polygons:
                    p = []
                    m = polygon.material_index
                    for vi in polygon.vertices:
                        p.append(vi + verti)
                    
                    a = 0
                    b = 0
                    tmp = 0
                    for pi in range(0, len(p)):
                        if (pi == 0):
                            a = p[0] + verti
                            b = p[len(p) - 1] + verti
                        else:
                            a = p[pi-1] + verti
                            b = p[pi] + verti
                            
                        if (a < b):
                            add_if_not_present([a, b], lin, m, lc)
                        else:
                            add_if_not_present([b, a], lin, m, lc)
                    
                    base = p[0]
                    for pi in range(2, len(p)):
                        tri.append([base+verti, p[pi-1]+verti, p[pi]+verti])
                        pc.append(m)
                    
                    
                
        filename = self.filepath
        out = open(filename, "wb")
        
        out.write(struct.pack("<I", 1))
        out.write(struct.pack("<IIII", len(mat), len(ver), len(tri), len(lin)))
        for m in mat:
            out.write(struct.pack("<fff", m[0], m[1], m[2]))
        for v in ver:
            out.write(struct.pack("<fff", v[0], v[1], v[2]))
        for t in tri:
            out.write(struct.pack("<III", t[0], t[1], t[2]))
        for pci in pc:
            out.write(struct.pack("<I", pci))
        for l in lin:
            out.write(struct.pack("<II", l[0], l[1]))
        for lci in lc:
            out.write(struct.pack("<I", lci))
        
        out.close()
        return {'FINISHED'};






def menu_func_export(self, context):
    self.layout.operator(ExportTest.bl_idname, text="Binary Mesh v1 (.bm1)")


def register():
    bpy.utils.register_class(ExportTest)
    bpy.types.TOPBAR_MT_file_export.append(menu_func_export)


def unregister():
    bpy.utils.unregister_class(ExportTest)
    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)

if __name__ == "__main__":
    register()
