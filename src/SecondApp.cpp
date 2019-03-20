//
//  secondApp.cpp
//  CAS1
//
//  Created by Gunjan Bhutani on 3/10/19.
//
#include "ofxMSATensorFlow.h"
#include <SecondApp.h>

void SecondApp::setup(){
    ofBackground(0);
    ofSetVerticalSync(false);
    lol = 100;
    ofSetColor(255);
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetFrameRate(20); // generating a character per frame at 60fps is too fast to read in realtime! so reducing this to 20 as ghetto way of limiting the rate
    
    jujuBG.load("jujubg.png");
    myFontResult.load("SpaceMono-Regular.ttf", 18, true, true);
    fontLogo.load("MajorMonoDisplay-Regular.ttf", 84, true, true);
    jujuScore.load("MajorMonoDisplay-Regular.ttf", 44, true, true);
    
    heightS = ofGetHeight();
    widthS = ofGetWidth();
    
    // Setup post-processing chain
    postS.init(ofGetWidth(), ofGetHeight());
    postS.createPass<BloomPass>()->setEnabled(true);
    
    // scan models dir
    models_dir.listDir("models");
    if(models_dir.size()==0) {
        ofLogError() << "Couldn't find models folder." << msa::tf::missing_data_error();
        assert(false);
        ofExit(1);
    }
    models_dir.sort();
    load_model_index(0); // load first model
    
    // seed rng
    rng.seed(ofGetSystemTimeMicros());
    
}

//--------------------------------------------------------------
// Load model by folder INDEX
void SecondApp::load_model_index(int index) {
    cur_model_index = ofClamp(index, 0, models_dir.size()-1);
    load_model(models_dir.getPath(cur_model_index));
}


//--------------------------------------------------------------
// Load graph (model trained in and exported from python) by folder NAME, and initialize session
void SecondApp::load_model(string dir) {
    // init session with graph
    session = msa::tf::create_session_with_graph(dir + "/graph_frz.pb");
    
    if(!session) {
        ofLogError() << "Session init error." << msa::tf::missing_data_error();
        assert(false);
        ofExit(1);
    }
    
    // load character map
    load_chars(dir + "/chars.txt");
    
    // init tensor for input
    // needs to be a single int (index of character)
    // HOWEVER input is not a scalar or vector, but a rank 2 tensor with shape {1, 1} (i.e. a matrix)
    // WHY? because that's how the model was designed to make the internal calculations easier (batch size etc)
    // TBH the model could be redesigned to accept just a rank 1 scalar, and then internally reshaped, but I'm lazy
    t_data_in = tensorflow::Tensor(tensorflow::DT_INT32, {1, 1});
    
    // prime model
    prime_model(text_full, prime_length);
}


//--------------------------------------------------------------
// load character <-> index mapping
void SecondApp::load_chars(string path) {
    ofLogVerbose() << "load_chars : " << path;
    int_to_char.clear();
    char_to_int.clear();
    ofBuffer buffer = ofBufferFromFile(path);
    
    for(auto line : buffer.getLines()) {
        char c = ofToInt(line); // TODO: will this manage unicode?
        int_to_char.push_back(c);
        int i = int_to_char.size()-1;
        char_to_int[c] = i;
        ofLogVerbose() << i << " : " << c;
    }
}



//--------------------------------------------------------------
// prime model with a sequence of characters
// this runs the data through the model element by element, so as to update its internal state (stored in t_state)
// next time we feed the model an element to make a prediction, it will make the prediction primed on this state (i.e. sequence of elements)
void SecondApp::prime_model(string prime_data, int prime_length) {
    t_state = tensorflow::Tensor(); // reset initial state to use zeros
    for(int i=MAX(0, prime_data.size()-prime_length); i<prime_data.size(); i++) {
        run_model(prime_data[i], t_state);
    }
}



//--------------------------------------------------------------
// add character to string, manage ghetto wrapping for display, run model etc.
void SecondApp::add_char(char ch) {
    // add sampled char to text
    if(ch == '\n') {
        text_lines.push_back("");
    } else {
        text_lines.back() += ch;
    }
    
    // ghetto word wrap
    if(text_lines.back().size() > max_line_width) {
        string text_line_cur = text_lines.back();
        text_lines.pop_back();
        auto last_word_pos = text_line_cur.find_last_of(" ");
        text_lines.push_back(text_line_cur.substr(0, last_word_pos));
        text_lines.push_back(text_line_cur.substr(last_word_pos));
    }
    
    // ghetto scroll
    while(text_lines.size() > max_line_num) text_lines.pop_front();
    
    // rebuild text
    text_full.clear();
    for(auto&& text_line : text_lines) {
        text_full += "\n" + text_line;
    }
    
    
    // feed sampled char back into model
    run_model(ch, t_state);
}



void SecondApp::update(){
    
}

void SecondApp::draw(){
    //ofDrawCircle(100, 100, gui2->radius+100);
    postS.begin();
    
    ofSetColor(main->juju);
    
    if(main->jujuStart){
        jujuBG.draw(0,0);
        myFontResult.drawString("Calculating JUJU . . . ", 70, 1640);
        if(main->jujuTwo){
            ofSetColor(0);
            ofDrawRectangle(60, 1600, 100, 100);
            ofSetColor(main->juju);
            do_auto_run = true;
            jujuBG.draw(0,0);
            ofDrawBitmapString(ofToString(main->contPoints), 20, 20);
            
            stringstream str;
            str << ofGetFrameRate() << endl;
            str << endl;
            str << "ENTER : toggle auto run " << (do_auto_run ? "(X)" : "( )") << endl;
            str << "RIGHT : sample one char " << endl;
            str << "DEL   : clear text " << endl;
            str << endl;
            
            str << "Press number key to load model: " << endl;
            for(int i=0; i<models_dir.size(); i++) {
                auto marker = (i==cur_model_index) ? ">" : " ";
                str << " " << (i+1) << " : " << marker << " " << models_dir.getName(i) << endl;
            }
            
            str << endl;
            str << "Any other key to type," << endl;
            str << "(and prime the model accordingly)" << endl;
            str << endl;
            
            
            if(session) {
                // sample one character from probability distribution
                int cur_char_index = msa::tf::sample_from_prob(rng, last_model_output);
                char cur_char = int_to_char[cur_char_index];
                
                str << "Next char : " << cur_char_index << " | " << cur_char << endl;
                
                if(do_auto_run || do_run_once) {
                    if(do_run_once) do_run_once = false;
                    
                    add_char(cur_char);
                }
            }
            
            // display probability histogram
            //msa::tf::draw_probs(last_model_output, ofRectangle(0, 0, ofGetWidth(), ofGetHeight()));
            
            
            // draw texts
            ofSetColor(main->juju);
            //ofDrawBitmapString(str.str(), 20, 20);
            
            myFontResult.setLetterSpacing(1);
            ofSetColor(main->juju);
            myFontResult.drawString(text_full + "_", 70, 1620);
        }
            
            //ofDrawBitmapString(ofToString(main->contPoints) + "_", 360, 20);
            //main->cropSample.draw();
            ofPolyline connections(main->contPoints);
            ofDrawBitmapString(ofToString(main->contPoints), 80, 1400);
            ofDrawBitmapString(ofToString(main->contPoints), 80, 1420);
            ofDrawBitmapString(ofToString(main->contPoints), 80, 1440);
        
            jujuScore.drawString("juju level:" + ofToString(main->score - 10000), 70, 1555);

        for(int j = 0; j<20; j++){
                    ofPushMatrix();
                    ofScale(4-0.15*j);
                    ofSetLineWidth(5);
                    ofSetColor(main->juju,255-j*10);
                    ofTranslate(gui2->offsetX,gui2->offsetY+40*j);
                    ofRotateXDeg(gui2->rotate*j+0);
                        connections.draw();
                        for(int i = 0; i< main->contPoints.size(); i++){
                            ofDrawCircle(main->contPoints[i].x, main->contPoints[i].y, 2);
                        }
                    ofPopMatrix();
        }
        
    }
    
    else if(main->jujuOne){
        do_auto_run = false;
        ofSetColor(main->juju, 180);
        string logo = "JUJU";
        fontLogo.drawString(logo,widthS/2-fontLogo.stringWidth(logo)/2, heightS/2-fontLogo.stringHeight(logo)/2+40);
        ofSetColor(main->juju);
        string msg2 = "PLACE JUJU CARD TO BEGIN ANALYSIS";
        myFontResult.setLetterSpacing(1.8);
        myFontResult.drawString(msg2,widthS/2-myFontResult.stringWidth(msg2)/2, heightS/2-myFontResult.stringHeight(msg2)/2+80);
        string msg3 = "ANALYZING...";
        myFontResult.setLetterSpacing(1.8);
        myFontResult.drawString(msg3,widthS/2-myFontResult.stringWidth(msg3)/2, heightS/2-myFontResult.stringHeight(msg3)/2+180);
        text_lines = { "_" };
    }
    
    else{
        do_auto_run = false;
        ofSetColor(main->juju, 180);
        string logo = "JUJU";
        fontLogo.drawString(logo,widthS/2-fontLogo.stringWidth(logo)/2, heightS/2-fontLogo.stringHeight(logo)/2+40);
        ofSetColor(main->juju);
        string msg2 = "PLACE JUJU CARD TO BEGIN ANALYSIS";
        myFontResult.setLetterSpacing(1.8);
        myFontResult.drawString(msg2,widthS/2-myFontResult.stringWidth(msg2)/2, heightS/2-myFontResult.stringHeight(msg2)/2+80);
        text_lines = { "_" };
    }
    postS.end();
    
}


//--------------------------------------------------------------
void SecondApp::keyPressed(int key) {
    switch(key) {
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            load_model_index(key-'1');
            break;
            
        case OF_KEY_DEL:
            text_lines = { "The" };
            break;
            
        case OF_KEY_RETURN:
            do_auto_run ^= true;
            break;
            
        case OF_KEY_RIGHT:
            do_run_once = true;
            do_auto_run = false;
            break;
            
        default:
            do_auto_run = false;
            if(char_to_int.count(key) > 0) add_char(key);
            break;
    }
    
    }
