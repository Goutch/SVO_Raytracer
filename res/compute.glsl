#version 430
#define RIGHT 4
#define TOP 2
#define BACK 1
#define LEAF_MASK 0x10000000
#define MAX_DIST 100.0
#define CHUNK_SCALE 1.0

struct Node{
    uint type;
    uint parent;
    uint children[8];
};

layout(local_size_x = 1, local_size_y = 1) in;
layout(rgba32f, binding = 0) uniform image2D img_output;
layout(std430, binding = 1) buffer Nodes_input
{
    Node octree[];
};

uniform vec2 resolution;
uniform mat4 view_rotation;
uniform vec4 view_position;

int getChild(vec3 p, int depth)
{
    float radius = CHUNK_SCALE/float(1 << depth-1);
    p+=radius;
    int x_greater = int(p.x >= radius);
    int y_greater = int(p.y >= radius);
    int z_greater = int(p.z >= radius);
    int child = (x_greater * RIGHT) + (y_greater * TOP) + (z_greater * BACK);
    return child;
}


vec4 trace(vec3 o, vec3 d)
{

    Node current_node=octree[0];
    int depth=1;
    int child=getChild(o, depth);
    int x = (child>>2) & 1?0:1;
    int y = (child>>1) & 1?0:1;
    int z = child & 1? 0:1;
    int current_index=0;
    int current_node=octree[current_index].children[child];
    //current is not a leaf
    while (current & LEAF_MASK==0)
    {
        depth++;

    }
    vec3 p=o;

    vec3 max=vec3(
    CHUNK_SCALE/2.,
    CHUNK_SCALE/2.,
    CHUNK_SCALE/2.);
    vec3 min=vec3(
    -CHUNK_SCALE/2.,
    -CHUNK_SCALE/2.,
    -CHUNK_SCALE/2.);
    vec3 bound=vec3(
    d.x<0?max.x:min.x,
    d.y<0?max.y:min.y,
    d.z<0?max.z:min.z);

    float tx=(bound.x-o.x)/d.x;
    float ty=(bound.y-o.y)/d.y;
    float tz=(bound.z-o.z)/d.z;

    float t=MAX_DIST;

    p=((d*tz)+o);
    if (tz >= 0 &&
    p.x > min.x && p.x < max.x &&
    p.y > min.y && p.y < max.y)
    {
        return vec4(1.0, 1.0, 1.0, 1.0);
    }
    p=((d*ty)+o);
    if (ty >= 0 &&
    p.x > min.x && p.x < max.x &&
    p.z > min.z && p.z < max.z)
    {
        return vec4(1.0, 1.0, 1.0, 1.0);
    }
    p=((d*tx)+o);
    if (tx >= 0 &&
    p.y > min.y && p.y < max.y &&
    p.z > min.z && p.z < max.z)
    {
        return vec4(1.0, 1.0, 1.0, 1.0);
    }

    return vec4(d, 1.0);

    /*float t=0;
    int current_node=0;
    int depth=0;
    vec3 p=o;
    float bound=CHUNK_SCALE;
    while (t<CHUNK_SCALE){
        //go deeper until a leaf is found.
        while (current_node < LEAF_MASK)
        {
            depth +=1;

            current_node = octree[current_node].children[getChild(o,d,t,depth)];
        }
        //if it is air go next
        if(current_node-LEAF_MASK==0)
        {
            //reverse once position is outside of bounding.
            while ( abs(p.x) > bound ||
            abs(p.y) > bound ||
            abs(p.z) > bound)
            {
                //if outside of octree return default color
                depth -= 1;
                if (depth == -1)
                {
                    return vec4(d, 1.0);
                }
                current_node=octree[octree[current_node].parent];
            }
        }
        else
        {
            return vec4(1.,1.,1.,1.);
        }
    }
    return vec4(d, 1.0);*/
}

void main() {
    ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
    vec2 uv=gl_GlobalInvocationID.xy/resolution;

    vec4 dir= vec4(uv.x-0.5, uv.y-0.5, 1., 1.);
    dir=normalize(dir);
    dir=view_rotation*dir;


    // output to a specific pixel in the image
    imageStore(img_output, pixel_coords, trace(view_position.xyz, dir.xyz));
}
