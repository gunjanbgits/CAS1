//
//  secondScreen.h
//  CAS1
//
//  Created by Gunjan Bhutani on 3/10/19.
//
#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "GuiApp.h"
#include "ofApp.h"
#include "ofxMSATensorFlow.h"
#include "ofxPostProcessing.h"

class SecondApp: public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    
    int lol, heightS, widthS;
    ofImage jujuBG;
    ofTrueTypeFont myFontResult, fontLogo, jujuScore;
    ofxPostProcessing postS;
    
    shared_ptr<GuiApp> gui2;
    shared_ptr<ofApp> main;
    
    // shared pointer to tensorflow::Session
    // This is using the lower level C API
    // For the higer level C++ API (using msa::tf::SimpleModel) see example-pix2pix-simple
    msa::tf::Session_ptr session;
    
    
    // for managing character <-> index mapping
    vector<char> int_to_char;
    map<int, char> char_to_int;
    
    
    // tensors in and out of model
    tensorflow::Tensor t_data_in;   // data in
    tensorflow::Tensor t_state;     // current lstm state
    vector<tensorflow::Tensor> t_out; // returned from session run [ data_out (prob), state_out ]
    vector<float> last_model_output;    // probabilities
    
    
    // generated text
    // managing word wrap in very ghetto way
    string text_full;
    list<string> text_lines = { "The" };
    int max_line_width = 60;
    int max_line_num = 6;
    
    
    // model file management
    ofDirectory models_dir;    // data/models folder which contains subfolders for each model
    int cur_model_index = 0; // which model (i.e. folder) we're currently using
    
    
    // random generator for sampling
    std::default_random_engine rng;
    
    
    // other vars
    int prime_length = 50;
    float sample_temp = 0.5f;
    
    bool do_auto_run = false;    // auto run every frame
    bool do_run_once = false;   // only run one character

    
    void load_model_index(int index);
    void load_model(string dir);
    void load_chars(string path);
    void prime_model(string prime_data, int prime_length);
    void add_char(char ch);
    void keyPressed(int key);
    
    //--------------------------------------------------------------
    // run model on a single character
    void run_model(char ch, const tensorflow::Tensor &state_in = tensorflow::Tensor()) {
        // copy input data into tensor
        msa::tf::scalar_to_tensor(char_to_int[ch], t_data_in);
        
        // run graph, feed inputs, fetch output
        vector<string> fetch_tensors = { "data_out", "state_out" };
        tensorflow::Status status;
        if(state_in.NumElements() > 0) {
            // use state_in if passed in as parameter
            status = session->Run({ { "data_in", t_data_in }, { "state_in", state_in } }, fetch_tensors, {}, &t_out);
        } else {
            // otherwise model will use internally init state to zeros
            status = session->Run({ { "data_in", t_data_in }}, fetch_tensors, {}, &t_out);
        }
        
        if(status != tensorflow::Status::OK()) {
            ofLogError() << status.error_message();
            return;
        }
        
        // convert model output from tensors to more manageable types
        if(t_out.size() > 1) {
            last_model_output = msa::tf::tensor_to_vector<float>(t_out[0]);
            last_model_output = msa::tf::adjust_probs_with_temp(last_model_output, sample_temp);
            
            // save lstm state for next run
            t_state = t_out[1];
        }
    }
    
    
    
};



