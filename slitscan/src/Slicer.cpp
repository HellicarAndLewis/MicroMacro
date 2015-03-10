//
//  Slicer.cpp
//  slitscan
//
//  Created by Chris on 09/03/2015.
//
//

#include "Slicer.h"


void Slicer::setup(int w, int h){
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
}


void Slicer::update(){
    
    int sliceWidth = 20;
    int width = 0;
    // Mask FBO: draw white to generate the mask
    maskFbo.begin();
    while (width < maskFbo.getWidth()) {
        ofRect(width, 0, sliceWidth, maskFbo.getHeight());
        width += sliceWidth * 2;
    }
    maskFbo.end();
    
    // mask using the shader
    fbo.begin();
    ofClear(0, 0, 0, 0);
    shader.begin();
    shader.setUniformTexture("maskTex", maskFbo.getTextureReference(), 1 );
    // draw the img: this gets set externally using a video frame: need to optimise this bit
    srcImg.draw(0,0, fbo.getWidth(), fbo.getHeight());
    shader.end();
    fbo.end();
    
}


void Slicer::draw(int x, int y){
    ofSetColor(255,255);
    fbo.draw(x, y);
}


void Slicer::keyPressed(int key){
}


void Slicer::keyReleased(int key){
    
}


void Slicer::mouseMoved(int x, int y ){
    
}


void Slicer::mouseDragged(int x, int y, int button){
    
}


void Slicer::mousePressed(int x, int y, int button){
}


void Slicer::mouseReleased(int x, int y, int button){
}


void Slicer::windowResized(int w, int h){
    
}


void Slicer::gotMessage(ofMessage msg){
    
}


void Slicer::dragEvent(ofDragInfo dragInfo){
    
}
