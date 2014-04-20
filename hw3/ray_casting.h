/* This function will calculate the intersection point of a ray
*  and a triangle. 
*  
*  input: 
*         v1,v2,v3: the coordinate of the vertices of the triangle
*         p: starting point of the ray
*         l: direction vector of the ray
*
*  output: 
*         cg: the barycentic coordination of the interaction point
*             with respect to the triagle's vertices.
*/

void ray_cast(const float *v1, const float *v2, const float *v3, const float *p, const float *l, float
    *cg)
{ cg[0] = (v2[1] * l[2] * p[0] + l[1] * v2[0] * p[2] - v2[2] *
    l[1] * p[0] - l[2] * v2[0] * p[1] - l[0] * v2[1] * p[2] + l[0] *
    v2[2] * p[1] - v2[1] * l[2] * v3[0] - l[1] * v2[ 0] * v3[2] +
    v2[2] * l[1] * v3[0] + l[2] * v2[0] * v3[1] + l[0] * v2[1] * v3[2]
    - l[0] * v2[2] * v3[1] + l[1] * p[0] * v3[2] - v3[1] * l[2] * p[0]
    - l[1] * v3[0] * p[2] + p[1 ] * l[2] * v3[0] + l[0] * v3[1] * p[2]
    - l[0] * p[1] * v3[2]) / (v2[1] * l[2] * v1[0] - v2[1] * l[2] *
    v3[0] + l[1] * v2[0] * v1[2] + v1[1] * l[2] * v3[0] - l[2] * v2[0]
    * v1 [1] + l[2] * v2[0] * v3[1] - l[2] * v1[0] * v3[1] - l[0] *
    v1[1] * v3[2] + l[0] * v1[2] * v3[1] - l[0] * v2[1] * v1[2] + l[0]
    * v2[1] * v3[2] - l[0] * v2[2] * v3[1] - l[1] * v2[0] * v3[2] +
    l[1] * v1[0] * v3[2] - v1[2] * l[1] * v3[0] + v2[2] * l[1] * v3[0]
    - v2[2] * l[1] * v1[0] + l[0] * v2[2] * v1[1]); cg[1] = -(l[0] *
    v1[1] * v3[2] - l[0] * v1[1] * p[2] - l[0] * v1[2] * v3[1] + l[0]
    * v3[1] * p[2] + l[0] * v1[2] * p[1] - l[0] * p[1] * v3[2] - v1[2]
    * l[1] * p[0] - l[1] * v3[0] * p[2] - v3[1] * l[2] * p[0] + v1[1]
    * l[2] * p[0] + l[1] * v1[0] * p[2] - v1[1] * l[2] * v3[0] + l[1]
    * p[0] * v3[2] - l[1] * v1[0] * v3[2] + l[2] * v1[0] * v3[1] - l[2
    ] * v1[0] * p[1] + p[1] * l[2] * v3[0] + v1[2] * l[1] * v3[0]) /
    (v2[1] * l[2] * v1[0] - v2[1] * l[2] * v3[0] + l[1] * v2[0] *
    v1[2] + v1[1] * l[2] * v3[0] - l[2] * v2[0] * v1[1] + l[2] *
    v2[0] * v3[1] - l[2] * v1[0] * v3[1] - l[0] * v1[1] * v3[2] + l[0]
    * v1[2] * v3[1] - l[0] * v2[1] * v1[2] + l[0] * v2[1] * v3[2] -
    l[0] * v2[2] * v3[1] - l[1] * v2[0] * v3[2] + l[1] * v1[0] * v3[2]
    - v1[2] * l[1] * v3[0] + v2[2] * l[1] * v3[0] - v2[2] * l[1] *
    v1[0] + l[0] * v2[2] * v1[1]); cg[2] = (-l[2] * v2[0] * v1[1] +
    l[2] * v2[0] * p[1] - l[2] * v1[0] * p[1] + l[0] * v2[1] * p[2] +
    v1[1] * l[2] * p[0] + v2[1] * l[2] * v1[0] - v2[1] * l[2] * p[0] +
    l[1] * v2[0] * v1[2] - l[1] * v2[0] * p[2] + l[1] * v1[0] * p[2]
    - v1[2] * l[1] * p[0] + v2[2] * l[1] * p[0] - v2[2] * l[1] * v1[0]
    - l[0] * v2[1] * v1[2] - l[0] * v1[1] * p[2] + l[0 ] * v1[2] *
    p[1] + l[0] * v2[2] * v1[1] - l[0] * v2[2] * p[1]) / (v2[1] * l[2]
    * v1[0] - v2[1] * l[2] * v3[0] + l[1] * v2[0] * v1[2] + v1[1] *
    l[2] * v3[0] - l[2] * v2[0] * v1[1] + l[2] * v2[0] * v3[1] - l[2]
    * v1[0] * v3[1] - l[0] * v1[1] * v3[2] + l[0] * v1[2] * v3[1] -
    l[0] * v2[1] * v1[2] + l[0] * v2[1] * v3[2] - l[0] * v2[2] * v3[1]
    - l[1] * v2[0] * v3[2] + l[1] * v1[0] * v3[2] - v1[2] * l[1] *
    v3[0] + v2[2] * l[1] * v3[0] - v2[2] * l[1] * v1[0] + l[0] * v2[2]
    * v1[1]); 
}