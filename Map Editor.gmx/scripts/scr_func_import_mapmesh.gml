var filepath = get_open_filename("Platform64 Map Mesh data (*.p64maped)","p64maped");
if (filepath != "")
{
    var file = file_text_open_read(filepath); 
    
    var verts; verts[0, 0] = 0;
    var vcolors; vcolors[0] = 0;
    var faces; faces[0, 0] = 0;
    
    var vertcount = 0;
    var facecount = 0;
    
    var status = "";
    
    var character = "";
    var lastchar = "";
    
    while (!file_text_eof(file))
    {
        var text = file_text_read_string(file);
        var text_size = string_length(text);
        
        for (i=0; i<text_size; i++)
        {
            lastchar = character;
            character = string_char_at(text, i+1);
            
            // Comment block handling
            if (lastchar == "/" && character == "*")
            {
                status = "Comment Block";
                continue;
            }
            if (status == "Comment Block")
            {
                if (lastchar == "*" && character == "/")
                    status = "";
                continue;
            }
            
            if (lastchar == "/" && character == "/")
                break;
            
            // Vertex handling
            if (character == "v" && status == "")
            {
                var strsplit = string_split(text, " ");
                verts[@vertcount, 0] = real(strsplit[1]);
                verts[@vertcount, 1] = -real(strsplit[2]);
                verts[@vertcount, 2] = real(strsplit[3]);
                vcolors[@vertcount] = make_color_rgb(real(strsplit[4])*255, real(strsplit[5])*255, real(strsplit[6])*255);
                vertcount++;
                status = "";
                break;
            }
            
            // Face handling
            if (character == "f" && status == "")
            {
                var strsplit = string_split(text, " ");
                faces[@facecount, 0] = real(strsplit[1]);
                faces[@facecount, 1] = real(strsplit[2]);
                faces[@facecount, 2] = real(strsplit[3]);
                facecount++;
                status = "";
                break;
            }
        }
        file_text_readln(file);
    }
    file_text_close(file);
    
    var obj = instance_create(0, 0, obj_mapmesh);
    with (obj)
    {
        self.verts = verts;
        self.vcolors = vcolors;
        self.faces = faces;
        self.vertcount = vertcount;
        self.facecount = facecount;
        event_user(0);
    }
}

