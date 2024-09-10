## Building

Windows: Use Visual Studio

Mac:

```
g++ -std=c++20 -o raytracer Raytracer.cpp
```

## Using

Windows:

```
PathToRaytracer.exe | Set-Encoding output.ppm (incomplete command)
```

Mac:

```
./raytracer > output.ppm
```

## Combine everything

Mac:

```
fswatch -o *.cpp *.h  | xargs -n1 sh -c 'make && ./raytracer > out.ppm && open out.ppm'
```
