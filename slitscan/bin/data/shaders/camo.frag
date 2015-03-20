#version 120

uniform sampler2DRect tex0;
uniform sampler2DRect camoText;
uniform vec2 rOffset;
uniform vec2 gOffset;
uniform vec2 bOffset;


// 169, 147, 30
// 115, 115, 51
// 92, 105, 50
// 73, 44, 50

void main () {
    vec4 colour = texture2DRect(tex0, gl_TexCoord[0].st);
    vec4 camo = texture2DRect(camoText, gl_TexCoord[0].st);
    // brown default
    vec4 tone = vec4(73.0/255, 44.0/255, 50.0/255, 1.0);
    if (camo.r > 0.5) {
        tone = vec4(115.0/255, 115.0/255, 51.0/255, 1.0);
    }
    if (camo.r > 0.7) {
        tone = vec4(92.0/255, 105.0/255, 50.0/255, 1.0);
    }
    if (camo.r > 0.8) {
        tone = vec4(169.0/255, 147.0/255, 30.0/255, 1.0);
    }
    
    if (camo.r > 0.4) {
        //gl_FragColor = colour * tone;
        gl_FragColor = tone;
    }
    else {
        gl_FragColor = colour;
    }
    
}