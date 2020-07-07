#!/bin/sh

glslangValidator -V shader.vert.glsl -o vert.spv
glslangValidator -V shader.frag.glsl -o frag.spv
