#version 330

#ifdef VERTEX_SHADER

layout(location= 0) in vec3 position;
uniform mat4 mvpMatrix;

uniform mat4 mvMatrix;
uniform mat4 vpMatrix;

uniform mat4 m;
out vec3 vertex_position;
uniform mat4 normalMatrix;

layout(location= 2) in vec3 normal;
out vec3 vertex_normal;

void main( )
{
    gl_Position= vpMatrix * m * vec4(position, 1);

    vertex_position= vec3(mvMatrix * vec4(position, 1)) ;

    vertex_normal= vec3(normalMatrix * vec4(normal,0));

}

#endif


#ifdef FRAGMENT_SHADER


in vec3 vertex_position;

in vec3 vertex_normal;

uniform vec4 mesh_color= vec4(0.439, 0.329, 0.243, 1);

uniform sampler2D diffuseLeaf;
uniform sampler2D diffuseBark;

out vec4 fragment_color;
void main( )
{
    vec4 color= mesh_color;

    vec3 normal;
    normal= normalize(vertex_normal);

    float cos_theta;

    vec3 light       = vec3(0.38,-1,0);
    vec4 light_color = vec4(1,1,1,1) * 1.2;
    
//    cos_theta= abs(dot(normal, normalize(light - vertex_position)));        // eclairage "double face"
    cos_theta= max(0, dot(normal, normalize(-light)));         // eclairage, uniquement des faces bien orientees
    color= color * (1.0/3.14) * cos_theta * light_color ;

    fragment_color= color ;
}
#endif
