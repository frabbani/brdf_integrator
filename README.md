# brdf_integrator
Simple utility that convolve a hemisphere to simulate a BRDF to determine it's surface area. The purpose of this utility is to give a rough approximation of the normalization factor for a given brdf.

The utility reads and evaluates the BRDF from the expression file "brdf.txt" (https://github.com/zserge/expr/blob/master/README.md)

Example expression
```
// gaussian BRDF
m = 0.5
w = acos( dotr() ) / m
out = exp( -( w * w ) )
```

The output is captured in the variable 'out'. The input funciton 'dotr()' is the inner product of the view vector and the light reflection vector (V.R in the image below). 

![330px-Blinn_Vectors svg](https://user-images.githubusercontent.com/7139511/222979465-04a40731-bfe6-42a4-8e0d-8548092a22c3.png)
