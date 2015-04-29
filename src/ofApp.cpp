//
//  ofApp.cpp
//  NoiseGrid

//  Frequency by Andreas Müller
//
//  Comments by Glenna Xie on 4/28/15.
//
//

#include <ofApp.h>

void ofApp::setup(){
    //load a new font with a font size of 8
    fontSmall.loadFont("Fonts/DIN.otf", 8 );
    
    //use the font for gui
    ofxGuiSetFont( "Fonts/DIN.otf", 8 );
    //set gui window width
    ofxGuiSetDefaultWidth( 260 );
    
    //save gui setting to a xml file and reload it later
    string xmlSettingsPath = "Settings/NoiseGrid.xml";
    
    //set up gui
    gui.setup( "Noise Grid", xmlSettingsPath );
    
    gui.add( frequency.set("Frequency",  1.0f,  0.01f,  2.0f) );
    gui.add( timeFrequency.set("Time Frequency",  1.0f,  0.001f,  3.0f) );
    gui.add( magnitude.set("Magnitude",  0.3f,  0.0f,  5.0f) );
    gui.add( gridSize.set("Grid Size",  6.0f,  0.01f,  20.0f) );
    gui.add( gridResolution.set("Grid Resolution",  20,  3,  100) );
    
    //load xml to gui
    gui.loadFromFile( xmlSettingsPath );
    
    
    camera.setAutoDistance( false ); // ?
    
    //  setGlobalPosition { return getGlobalTransformMatrix().getTranslation(); }
    camera.setGlobalPosition( ofVec3f(0,5,-10) );
    camera.lookAt( ofVec3f(0,3,0) );  // ?
    camera.setNearClip( 0.0001f ); // ?
    camera.setFarClip( 512 );//  ?
    
    drawGui = true;
}

void ofApp::update(){
    
    float time = ofGetElapsedTimef();
    float noiseTime = time * timeFrequency;
    
    //decide how far each vertex extends
    ofVec3f extentMin( gridSize * -0.5, 0,		  gridSize * -0.5 );
    ofVec3f extentMax( gridSize *  0.5, gridSize, gridSize *  0.5 );
    
    //clear mesh as it draws
    mesh.clear();
    mesh.setMode( OF_PRIMITIVE_LINES );
    
    //go through each column of the grid
    for( int y = 0; y < gridResolution; y++ )
    {
        for( int x = 0; x < gridResolution; x++ )
        {
            for( int z = 0; z < gridResolution; z++ )
            {
                ofVec3f pos;
                
                //map the position of each vertex to the range that is defined earlier
                pos.x = ofMap( x,  0, gridResolution-1, extentMin.x, extentMax.x );
                pos.y = ofMap( y,  0, gridResolution-1, extentMin.y, extentMax.y );
                pos.z = ofMap( z,  0, gridResolution-1, extentMin.z, extentMax.z );
                
                // a new pos that is driven by the frequency
                ofVec3f noisePos = pos * frequency;
                
                ofVec3f vel;
                
                //ofSignedNoise returns a value between -1 and 1
                vel.x = ofSignedNoise( noisePos.x, noisePos.y, noisePos.z, noiseTime );
                vel.y = ofSignedNoise( noiseTime,  noisePos.z, noisePos.y, noisePos.x );
                vel.z = ofSignedNoise( noisePos.z, noiseTime,  noisePos.y, noisePos.x );
                
                //normalize a vector means to take a vector of any length and keeping it pointing in the same direction, change its length to 1, turning it into what is called a unit vector. The magnitude of a normalized vector is always 1.
                ofVec3f normVel = vel.getNormalized();
                
                //ofVec3f(1) is the same as ofVec3f(1,1,1)
                ofVec3f velAsCol = (normVel + ofVec3f(1)) * 0.5;
              
                // Color by direction
                // ofFloatColor returns floating pointing numbers between 0 and 1
                ofFloatColor tmpCol( velAsCol.x, velAsCol.y, velAsCol.z );
                
                //add the static vertex to the mesh
                mesh.addVertex( pos );
                mesh.addColor( tmpCol );
                
                //add the changing vertices to the mesh
                mesh.addVertex( pos + (vel * magnitude));
                
              // The ofFloatColor and ofShortColor represent higher precision representations of a given color and are used in special cases where precision color math is required. ofFloatColor might be used for specialized image processing, such as image averaging where rounding errors might otherwise introduce unaccaptable visual artifacts.
           
                mesh.addColor( ofFloatColor(tmpCol, 0.0) );
            }
        }
    }
    
    
}


void ofApp::draw(){
    //render a circular gradient background of gray and black
    ofBackgroundGradient( ofColor(40,40,40), ofColor(0,0,0), OF_GRADIENT_CIRCULAR);
    
    
    ofEnableDepthTest();
    
    camera.begin();
    
    // draw a grid on the floor
    ofSetColor( ofColor(60) );
    ofPushMatrix();
    ofRotate(90, 0, 0, -1);
    ofDrawGridPlane( 10, 10, false );
    ofPopMatrix();
    
    //reset the color
    ofSetColor( ofColor::white );
				
    mesh.draw();
    
    camera.end();
    
    
    ofDisableDepthTest();
    
    
    if( drawGui ) { gui.draw(); }
    
    //draw the text
    ofSetColor( ofColor::black );
    fontSmall.drawString( ofToString(ofGetFrameRate(), 1), ofGetWidth() - 25, ofGetHeight() - 5 );
}

void ofApp::keyPressed( int _key ){
    
    if( _key == ' ' )			  { }
    //enable gui using the TAB key
    else if( _key == OF_KEY_TAB ) { drawGui = !drawGui; }
    
    //enable fullscreen using the f key
    else if( _key == 'f' )		  { ofToggleFullscreen(); }
    
}

