//
//  AlphaMask.cpp
//  slitscan
//
//  Created by Chris Mullany on 20/03/2015.
//
//

#include "AlphaMask.h"

void AlphaMask::setup(int w, int h){
    
    ofEnableAlphaBlending();
    maskFbo.allocate(w,h);
    fbo.allocate(w,h);
    
    // Load alpha shader from ofx examples
    if (ofGetGLProgrammableRenderer()) {
        string vertex = "#version 150\n\
        \n\
        uniform mat4 projectionMatrix;\n\
        uniform mat4 modelViewMatrix;\n\
        uniform mat4 modelViewProjectionMatrix;\n\
        \n\
        \n\
        in vec4  position;\n\
        in vec2  texcoord;\n\
        \n\
        out vec2 texCoordVarying;\n\
        \n\
        void main()\n\
        {\n\
        texCoordVarying = texcoord;\
        gl_Position = modelViewProjectionMatrix * position;\n\
        }";
        string fragment = "#version 150\n\
        \n\
        uniform sampler2DRect tex0;\
        uniform sampler2DRect maskTex;\
        in vec2 texCoordVarying;\n\
        \
        out vec4 fragColor;\n\
        void main (void){\
        vec2 pos = texCoordVarying;\
        \
        vec3 src = texture(tex0, pos).rgb;\
        float mask = texture(maskTex, pos).r;\
        \
        fragColor = vec4( src , mask);\
        }";
        shader.setupShaderFromSource(GL_VERTEX_SHADER, vertex);
        shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragment);
        shader.bindDefaults();
        shader.linkProgram();
    }
    else {
        string shaderProgram = "#version 120\n \
        #extension GL_ARB_texture_rectangle : enable\n \
        \
        uniform sampler2DRect tex0;\
        uniform sampler2DRect maskTex;\
        \
        void main (void){\
        vec2 pos = gl_TexCoord[0].st;\
        \
        vec3 src = texture2DRect(tex0, pos).rgb;\
        float mask = texture2DRect(maskTex, pos).r;\
        \
        gl_FragColor = vec4( src , mask);\
        }";
        shader.setupShaderFromSource(GL_FRAGMENT_SHADER, shaderProgram);
        shader.linkProgram();
    }
    
    // clear FBOs
    maskFbo.begin();
    ofClear(0,0,0,255);
    maskFbo.end();
    fbo.begin();
    ofClear(0,0,0,255);
    fbo.end();
    
    refresh();
}


void AlphaMask::update(){}


void AlphaMask::begin() {
    // mask using the shader passing in the mask FBO texture
    fbo.begin();
    ofClear(0, 0, 0, 0);
    shader.begin();
    shader.setUniformTexture("maskTex", maskFbo.getTextureReference(), 1 );
}

void AlphaMask::end() {
    shader.end();
    fbo.end();
}

void AlphaMask::beginMask() {
    // mask using the shader passing in the mask FBO texture
    maskFbo.begin();
    ofClear(0, 0, 0, 0);
}

void AlphaMask::endMask() {
    maskFbo.end();
}


void AlphaMask::draw(int x, int y){
    fbo.draw(x, y);
}

void AlphaMask::refresh(){
}

void AlphaMask::keyPressed(int key){
}


void AlphaMask::keyReleased(int key){
    
}


void AlphaMask::mouseMoved(int x, int y ){
    
}


void AlphaMask::mouseDragged(int x, int y, int button){
    
}


void AlphaMask::mousePressed(int x, int y, int button){
}


void AlphaMask::mouseReleased(int x, int y, int button){
}


void AlphaMask::windowResized(int w, int h){
    
}


void AlphaMask::gotMessage(ofMessage msg){
    
}


void AlphaMask::dragEvent(ofDragInfo dragInfo){
    
}
