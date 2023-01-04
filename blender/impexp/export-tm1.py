#!BPY

"""
.tm1 (Text Mesh version 1)
-------------
(no newline between the blocks)
M (material count)
V (vertex count)
T (triangle count)
L (line count)
M0Red M0Green M0Blue (diffuse)
...
V0x V0y V0z
V1x V1y V1z
..
T0a T0b T0c (triangle indices, length: 3)
...
C0 (triangle material indices)
C1
...
L0a L0b (line indices (start with 0))
C0 (material indices for lines)
C1
...
"""

bl_info = {
    "name": "Export-tm1",
    "author": "Curved Ruler",
    "version": (1, 1),
    "blender": (2, 74, 0),
    "location": "File > Export > tm1",
    "description": "Export to format: Text Mesh v1",
    "warning": "",
    "wiki_url": "",
    "tracker_url": "",
    "category": "Curved Ruler"
}

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
    """Export tm1"""
    
    bl_idname       = "example.tm1";
    bl_label        = "Export TM01";
    bl_options      = {'PRESET'};
    
    filename_ext    = ".tm1";
    
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
            if ob.select and ob.type == 'MESH':
                mat_w = ob.matrix_world
                verti = len(ver)
                mesh = ob.to_mesh(context.scene, True, 'PREVIEW')
                
                for vertex in mesh.vertices:
                    pos = mat_w * vertex.co
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
        out = open(filename, "w")
        out.write("{0}\n".format(len(mat)))
        out.write("{0}\n".format(len(ver)))
        out.write("{0}\n".format(len(tri)))
        out.write("{0}\n".format(len(lin)))
        for m in mat:
            out.write("{0} {1} {2}\n".format(m[0], m[1], m[2]))
        for v in ver:
            out.write("{0} {1} {2}\n".format(v[0], v[1], v[2]))
        for t in tri:
            out.write("{0} {1} {2}\n".format(t[0], t[1], t[2]))
        for pci in pc:
            out.write("{0}\n".format(pci))
        for l in lin:
            out.write("{0} {1}\n".format(l[0], l[1]))
        for lci in lc:
            out.write("{0}\n".format(lci))
        out.close()
        return {'FINISHED'};






def menu_func_export(self, context):
    self.layout.operator(ExportTest.bl_idname, text="TXT Mesh v1 (.tm1)")


def register():
    bpy.utils.register_module(__name__)
    bpy.types.INFO_MT_file_export.append(menu_func_export)


def unregister():
    bpy.utils.unregister_module(__name__)
    bpy.types.INFO_MT_file_export.remove(menu_func_export)

if __name__ == "__main__":
    register()
