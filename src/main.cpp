#include "ofMain.h"
#include "ofApp.h"
#include "GuiApp.h"
#include "SecondApp.h"
#include "ofAppGLFWWindow.h"

//========================================================================
int main( ){
//    ofSetupOpenGL(1280,800,OF_FULLSCREEN);            // <-------- setup the GL context
//
//    // this kicks off the running of my app
//    // can be OF_WINDOW or OF_FULLSCREEN
//    // pass in width and height too:
//    ofRunApp(new ofApp());
    
    ofGLFWWindowSettings settings;
    settings.monitor = 1;
    settings.setSize(1280, 800);
    settings.windowMode = OF_FULLSCREEN;
    shared_ptr<ofAppBaseWindow> mainWindow = ofCreateWindow(settings);
    
    settings.monitor = 2;
    settings.windowMode = OF_FULLSCREEN;
    settings.setSize(1080, 1920);
    shared_ptr<ofAppBaseWindow> secondWindow = ofCreateWindow(settings);
    
    //settings.monitor = 0;
    settings.windowMode = OF_WINDOW;
    settings.setSize(480, 800);
    settings.resizable = false;
    shared_ptr<ofAppBaseWindow> guiWindow = ofCreateWindow(settings);
    
    shared_ptr<ofApp> mainApp(new ofApp);
    shared_ptr<SecondApp> secondApp(new SecondApp);
    shared_ptr<GuiApp> guiApp(new GuiApp);
    
    //shared_ptr<GuiApp> guiApp2(new GuiApp);
    
    mainApp->gui = guiApp;
    secondApp->gui2 = guiApp;
    secondApp->main = mainApp;
    
    //guiApp->main = mainApp;
    //mainApp->second = secondApp;
    
    ofRunApp(guiWindow, guiApp);
    ofRunApp(secondWindow, secondApp);
    ofRunApp(mainWindow, mainApp);
    ofRunMainLoop();

}
