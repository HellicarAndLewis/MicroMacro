#version 120

uniform sampler2DRect tex0;
uniform vec2 rOffset;
uniform vec2 gOffset;
uniform vec2 bOffset;

void main () {
    vec4 rValue = texture2DRect(tex0, gl_TexCoord[0].st - rOffset);
    vec4 gValue = texture2DRect(tex0, gl_TexCoord[0].st - gOffset);
    vec4 bValue = texture2DRect(tex0, gl_TexCoord[0].st - bOffset);
    // Combine the offset colors.
    gl_FragColor = vec4(rValue.r, gValue.g, bValue.b, 1.0);
}