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
    setThickness();
}


void Slicer::update(){}


void Slicer::begin() {
    // mask using the shader passing in the mask FBO texture
    fbo.begin();
    ofClear(0, 0, 0, 0);
    shader.begin();
    shader.setUniformTexture("maskTex", maskFbo.getTextureReference(), 1 );
}

void Slicer::end() {
    shader.end();
    fbo.end();
}


void Slicer::draw(int x, int y){
    fbo.draw(x, y);
}


void Slicer::setThickness(int thickness){
    // sets thickness of slits by drawing white into the mask FBO
    // mask FBO is used to mask a given texture using being() and end()
    int width = 0;
    maskFbo.begin();
    ofClear(0,0,0,255);
    while (width < maskFbo.getWidth()) {
        ofRect(width, 0, thickness, maskFbo.getHeight());
        width += thickness * 2;
    }
    maskFbo.end();
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
