#!/bin/sh

glslc -fshader-stage=vertex vert.glsl -o vert.spv
glslc -fshader-stage=fragment frag.glsl -o frag.spv
glslc -fshader-stage=vertex vert2.glsl -o vert2.spv
glslc -fshader-stage=fragment frag2.glsl -o frag2.spv
