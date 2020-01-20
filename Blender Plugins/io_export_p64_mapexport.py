# TODO:
# - Implement UV Coordinates
# - Implement Vertex Normals
# - Implement separating model by textures
# - Implement triangle grouping
# - Implement non destructive model export (triangulation, object mode, only selected model, etc...)
# - Make collision exporter

bl_info = {
    "name": "Platform64 Map Export",
    "description": "Exports a map model for Platform64's map editor.",
    "author": "Buu342",
    "version": (1, 0),
    "blender": (2, 77, 0),
    "location": "File > Export > P64MAPED",
    "warning": "",
    "wiki_url": "",
    "tracker_url": "",
    "support": 'COMMUNITY',
    "category": "Import-Export"
}

import bpy
import bmesh
import struct #https://docs.python.org/3/library/struct.html
from bpy import context
from collections import defaultdict

def triangulateObject(obj):
    me = obj.data
    bm = bmesh.new()
    bm.from_mesh(me)
    bmesh.ops.triangulate(bm, faces=bm.faces[:], quad_method=0, ngon_method=0)
    bm.to_mesh(me)
    bm.free()
    
    
def generateVertColorMap(mesh):
    polygons = mesh.polygons

    vertex_map = defaultdict(list)
    for poly in polygons:
        for v_ix, l_ix in zip(poly.vertices, poly.loop_indices):
            vertex_map[v_ix].append(l_ix)
    return vertex_map;
    
# TODO: Add error checking
def getVertColor(mesh, vertex_map, vert):
    for v_ix, l_ixs in vertex_map.items():
        try:
            return mesh.vertex_colors['Col'].data[vertex_map[vert][0]].color
        except IndexError:
            return (1.0, 1.0, 1.0)
        except KeyError:
            return (1.0, 1.0, 1.0)
 
def writeObject(self, context):
    obj = context.active_object
    mesh = obj.data
    vertex_map = generateVertColorMap(mesh)
    
    with open(self.filepath, 'w') as f:
        f.write("/**********************************\n")
        f.write("    Platform 64 Map Editor Mesh\n")
        f.write("         Script by Buu342\n")
        f.write("            Version 1.0\n")
        f.write("**********************************/\n\n")
        f.write("// Verticies - v1 v2 v3 c1 c2 c3\n")
        f.write("// Where v is vertex coordinate, c is color\n")
        for v in mesh.vertices:
            f.write("v %.4f %.4f %.4f" % v.co[:])
            f.write(" %.4f %.4f %.4f" % getVertColor(mesh, vertex_map, v.index)[:])
            f.write("\n")
        f.write("\n")
        
        f.write("// Faces - v1 v2 v3\n")
        f.write("// Where v is vertex number, starting at 1\n")
        for p in mesh.polygons:
            f.write("f")
            for i in p.vertices:
                f.write(" %d" % (i + 1))
            f.write("\n")
        self.report({'INFO'}, 'File exported sucessfully!')
    return {'FINISHED'}


class ObjectExport(bpy.types.Operator):
    """Exports a map model for Platform64's map editor"""
    bl_idname = "object.export_p64maped"
    bl_label = "Platform64 Map Export" # The text on the export button
    bl_options = {'REGISTER', 'UNDO'}
    filename_ext = ".p64maped"
    
    filter_glob         = bpy.props.StringProperty(default="*.p64maped", options={'HIDDEN'}, maxlen=255)
    #setting_selected    = bpy.props.BoolProperty(name="Selected only", description="Export selected mesh items only", default=True)
    setting_triangulate = bpy.props.BoolProperty(name="Triangulate", description="Triangulate object", default=True)

    filepath = bpy.props.StringProperty(subtype='FILE_PATH')    
    
    def execute(self, context):
        self.filepath = bpy.path.ensure_ext(self.filepath, ".p64maped")
        if (self.setting_triangulate):
            triangulateObject(context.active_object)
            
        writeObject(self, context);        
        return {'FINISHED'}

    def invoke(self, context, event):
        if not self.filepath:
            self.filepath = bpy.path.ensure_ext(bpy.data.filepath, "untitled.p64maped")
        context.window_manager.fileselect_add(self)
        return {'RUNNING_MODAL'}



# Add trigger into a dynamic menu
def menu_func_export(self, context):
    self.layout.operator(ObjectExport.bl_idname, text="Platform64 Map (.p64maped)")
    

def register():
    bpy.utils.register_class(ObjectExport)
    bpy.types.INFO_MT_file_export.append(menu_func_export)


def unregister():
    bpy.utils.unregister_class(ObjectExport)
    bpy.types.INFO_MT_file_export.remove(menu_func_export)


if __name__ == "__main__":
    register()