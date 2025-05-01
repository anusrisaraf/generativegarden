#version 150

out vec4 fragColor;

uniform float time;
uniform float windStrength;
uniform int nightMode;

void main() {
    vec3 baseColor = nightMode == 1 ? vec3(0.1, 0.2, 0.3) : vec3(0.8, 0.9, 1.0);
    float flicker = 0.02 * sin(time * 5.0 + gl_FragCoord.x * 0.1);
    fragColor = vec4(baseColor + flicker, 1.0);
}
