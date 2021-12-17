#version 330

#ifdef VERTEX_SHADER

layout(location= 0) in vec3 position;
uniform mat4 mvpMatrix;

uniform mat4 mvMatrix;
uniform mat4 vpMatrix;

uniform mat4 m;
out vec3 vertex_position;
uniform mat4 normalMatrix;
uniform mat4 scale;

layout(location= 2) in vec3 normal;
out vec3 vertex_normal;

layout(location= 1) in vec2 texcoord;
out vec2 vertex_texcoord;

layout(location= 3) in int matID;
out int vertex_matID;


layout(location= 3) in vec4 color;
out vec4 vertex_color;


void main( )
{
    gl_Position= vpMatrix * m * vec4(position, 1) ;

    vertex_position= vec3(mvMatrix * vec4(position, 1)) ;

    vertex_normal= vec3(normalMatrix * vec4(normal,0));

    vertex_texcoord = texcoord;
}

#endif


#ifdef FRAGMENT_SHADER


in vec3 vertex_position;

in vec3 vertex_normal;

uniform vec4 mesh_color= vec4(0.54, 0.6, 0.35, 1);

uniform sampler2D diffuseLeaf;
uniform sampler2D diffuseBark;

out vec4 fragment_color;

uniform vec4 baseColor;
in vec2 vertex_texcoord;

flat in int vertex_matID;

void main( )
{
    vec4 color= mesh_color;

    vec3 normal;
    normal= normalize(vertex_normal);

    float cos_theta;

    vec3 light       = vec3(0.6,-1,0);
    vec4 light_color = vec4(1) * 2.0 ;

    vec4 colLeaf = texture(diffuseLeaf, vertex_texcoord);
    
    cos_theta= abs(dot(normal, normalize(-light)));        // eclairage "double face"
//    cos_theta= max(0, dot(normal, normalize(-light)));         // eclairage, uniquement des faces bien orientees
    color= color * (4.0/3.14) * cos_theta * light_color ;

    if(colLeaf.a < 0.6) discard;
    fragment_color= colLeaf * color ;
}
#endif
