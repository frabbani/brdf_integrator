# brdf_integrator
Simple utility that convolve a hemisphere to simulate a BRDF to determine it's surface area. The purpose of this utility is to give a rough approximation of the normalization factor for a given brdf. The utility also generates a 3D model for the lobe (saved as "lobe.obj")

The utility reads and evaluates the BRDF from the expression file "brdf.txt" (see https://github.com/zserge/expr/blob/master/README.md for more info.)

Example expression
```
// gaussian BRDF
m = 0.5
w = acos( dotr() ) / m
out = exp( -( w * w ) )
```

The output for the expression is captured in the variable 'out'. The input funciton 'dotr()' is the inner product of the view vector and the light reflection vector ('V dot R' in the image below). 

![330px-Blinn_Vectors svg](https://user-images.githubusercontent.com/7139511/222979465-04a40731-bfe6-42a4-8e0d-8548092a22c3.png)

Output Example (Blender preview)

<img width="552" height="532" alt="image" src="https://github.com/user-attachments/assets/c3945738-2d57-4445-b22d-049d303880d2" />
