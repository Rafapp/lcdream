#version 330 core

out vec4 FragColor;

in vec2 uv;

uniform sampler2D screenTexture;
uniform float brightness;

void main() {
    vec2 flippedUV = vec2(uv.x, 1.0 - uv.y);
    vec3 color = texture(screenTexture, flippedUV).rgb;
    // color *= brightness;
    FragColor = vec4(color, 1.0);
}