FastEngine is lightweight CPU-based renderer that does the rendering without any additional tools then uses SDL3 to display the final rendered frame.

Build Instructions:

```git clone --recursive https://github.com/AlexSid0/PerlinNoise.git```

* If this was not done previously use ```git submodule update --init```

cd PerlinNoise

For Visual Studio:
```cmake -B build -S .```
```cmake --build build```

For Make files:
```cmake -B build -S . -G "MinGW Makefiles"```
```cmake --build build```
```.\build\PerlinNoise.exe```